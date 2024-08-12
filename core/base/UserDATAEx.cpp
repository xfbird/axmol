//---------------------------------------------------------------------------
// UserDataEx.cpp
#include "base/UserDataEx.h"
#include <sstream>
#include <iostream>

#include "openssl/aes.h"
#include "openssl/modes.h"
#include "openssl/rc4.h"

#include <stdexcept>
#include "base/Logging.h"
#include "base/Utils.h"
//------------------------------------------------------------
//       使用json 来进行处理   
#include "simdjson/simdjson.h"
// #include "zlib.h"
#include "fmt/format.h"
#include "base/ZipUtils.h"
#include "base/PaddedString.h"
#include <string_view>
#include <span>
#include "yasio/byte_buffer.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
//------------------------------------------------------------

NS_AX_BEGIN
std::string UserDataEx::_storagename = "ken";
DataExMap UserDataEx::_dictionaryemap;                      //词典表

UserDataEx::UserDataEx() :
    _dataReady(false),
    _isModified(false),
    _autoSave(false),
    _encryptEnabled(false),
    _udname(""),
    _key(""),
    _iv(""),
    _values({}) {}

UserDataEx::UserDataEx(std::string_view sname) : UserDataEx() {
    if (!sname.empty()) {
        _udname = sname;
        //AXLOGD("UserDataEx sname:{} this:{}", sname, fmt::ptr(this));
        getUserDatainfoFromDefault();
    }
}

bool UserDataEx::getUserDatainfoFromDefault(){
    //从 UserDefault 获得 持久数据加载
    auto rkey=_storagename+std::string(_udname);       //组合主键       
    if (!rkey.empty()) {
        //AXLOGD("getUserDatainfoFromDefault 1 rkey:{} this:{}", rkey, fmt::ptr(this));
        auto ud =UserDefault::getInstance();
        if (ud){
            auto sstrv=ud->getStringForKey(rkey.data());   //获得主键。
            //AXLOGD("getUserDatainfoFromDefault 2 rkey:{} read:{} this:{}", rkey,sstrv,fmt::ptr(this));
            //直接返回 读出的字符串 并解析 到内部
            if (!sstrv.empty()) 
                return initFromString(sstrv);
            _values.clear();
            //AXLOGD("getUserDatainfoFromDefault 3 rkey:{} this:{} 读出为空,初始化", 
            //            rkey,fmt::ptr(this));
            return true;    
        }
    }
    return false;
}

UserDataEx::~UserDataEx() {
    if (_isModified) {
        // Consider moving save logic outside of destructor or using RAII
        saveData();
    }
}

    void modify();
void UserDataEx::modify() {
    _isModified=true;                                          //是否修改标志
    if (_autoSave) {                                           //自动保存开关 开启的话，会自动调用 保存             
        saveData();
    }
}



bool UserDataEx::initFromString(std::string_view rhs){
    using namespace simdjson;
    //读取过程 Base64解码 解压缩 反序列化
    try
    {
        //AXLOGD("initFromString rhs:{} this:{}",rhs,fmt::ptr(this));
        std::string mdata(rhs);
        simdjson::padded_string strJson(mdata); // copies to a padded buffer
        ondemand::parser parser;
        ondemand::document settings = parser.iterate(strJson);
        bool encrypt= settings["Encrypt"];              //是否为 加密的
        if (encrypt) {
           std::string_view skey  = settings["key"]; 
           std::string_view siv  = settings["iv"]; 
           setEncryptEnabled(true,skey,siv);
        };
        std::string_view svdatas = settings["datas"];
        //AXLOGD("initFromString svdatas:{} this:{}", svdatas,fmt::ptr(this));
        _values.clear();
        if (svdatas.empty()) {
            return true;
        }
        auto comprData   = utils::base64Decode(svdatas);
        auto  uncomprData = ZipUtils::decompressGZ(std::span{comprData});
        std::string sjdata(reinterpret_cast<const char*>(uncomprData.data()), uncomprData.size());
        AXLOGD("initFromString Get Str:{}",sjdata);
        simdjson::padded_string strJson2(sjdata); // copies to a padded buffer
        // simdjson::padded_string strJson2(std::string("")); // copies to a padded buffer
        dom::parser parser2;
        dom::element parsed_json;
        // 解析JSON字符串
        
        auto parse_result = parser2.parse(strJson2);

        if (parse_result.error()) {
            AXLOGW("Parse error:{}",error_message(parse_result.error()));
            return 1;
        }
        parsed_json = parse_result.value();

        // 确保解析的JSON是一个对象
        if (!parsed_json.is_object()) {
           AXLOGW("Top-level JSON is not an object.");
           return 1;
        }

        // 获取名为"dict"的数组
        auto dict_array = parsed_json.at_key("dict").get_array();
        if (dict_array.error()) {
            // std::cerr << "Error accessing 'dict' array: " << dict_array.error().message() << std::endl;
            return 1;
        }

        for (dom::element dict : dict_array.value()) {
            if (!dict.is_object()) {
                AXLOGW("Element in 'dict' array is not an object.");
                continue;
            }

            auto svnameRes = dict.at_key("name").get_string();
            if (svnameRes.error()) {
                AXLOGW("Error getting 'name':{}",error_message(svnameRes.error()));
                continue;
            }
            auto svname = svnameRes.value();

            auto svvalueRes = dict.at_key("value").get_string();
            if (svvalueRes.error()) {
                AXLOGW("Error getting 'value': {}",error_message(svvalueRes.error()));
                continue;
            }
            auto svvalue = svvalueRes.value();

            auto etypeRes = dict.at_key("type").get_int64();
            if (etypeRes.error()) {
                AXLOGW("Error getting 'type': {}", error_message(etypeRes.error()));
                continue;
            }
            auto etype = static_cast<u_char>(etypeRes.value());
            AXLOGD("initFromString setTypeValueForKey svname:{}  svvalue:{} etypeRes:{} this:{}", svname, svvalue,etype,fmt::ptr(this));
            setTypeValueForKey(svname, svvalue, etype);
        }
        _dataReady=true;
        _isModified=false;
    }
    catch (std::exception& ex)
    {
        AXLOGE("initFromString: {} conent{}  ",_udname, rhs);
        //throw; // Rethrow after logging
        _dataReady  = true;
        _isModified = false;
    }
    return false;
};

//生成 一个具体 项 的 存储字符串
std::string UserDataEx::GenJsonString(){
    rapidjson::Document document;
    document.SetObject();

    // Add boolean value "Encrypt"
    auto sdata=GenJsonDictString();
    document.AddMember("Encrypt", _encryptEnabled, document.GetAllocator());
    if (_encryptEnabled) {        //存在加密 需要输出 密钥
        // Add string values "key", "iv", and "data"
        document.AddMember("key", rapidjson::Value().SetString(_key.data(),_key.size(), document.GetAllocator()), document.GetAllocator());
        document.AddMember("iv", rapidjson::Value().SetString(_iv.data(),_iv.size(), document.GetAllocator()), document.GetAllocator());
    }
    document.AddMember("datas", rapidjson::Value().SetString(sdata.data(),sdata.size(), document.GetAllocator()), document.GetAllocator());
    // Serialize JSON to a string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    return buffer.GetString();
}

//生成字典的 存储字符串
std::string UserDataEx::GenJsonDictString(){
    //生成过程 序列化 加密 压缩 Base64
    rapidjson::Document doc;
    doc.SetObject();
    // 创建一个数组
    rapidjson::Value dictArray(rapidjson::kArrayType);
    // 遍历_values并添加到数组
    for (const auto& [key, value] : _values) {
        rapidjson::Value dict(rapidjson::kObjectType);
        //不管什么类型 采用字符串模式 去存储        
        dict.AddMember("name", rapidjson::Value().SetString(key.data(), key.size(), doc.GetAllocator()), doc.GetAllocator());
        dict.AddMember("value", rapidjson::Value().SetString(value.asString().c_str(), value.asString().size(), doc.GetAllocator()), doc.GetAllocator());
        dict.AddMember("type", static_cast<int>(value.getType()), doc.GetAllocator());
        dictArray.PushBack(dict, doc.GetAllocator());
    }
    // 将数组添加到文档
    doc.AddMember("dict", dictArray, doc.GetAllocator());
    // 创建一个StringBuffer用于保存JSON字符串
    rapidjson::StringBuffer buffer;
    // 创建一个Writer用于写入StringBuffer
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    // 将文档写入StringBuffer
    doc.Accept(writer);
    // 返回构建的JSON字符串
    // Step 2: Encrypt the JSON string
    std::string jsonStr = buffer.GetString();
    if (_encryptEnabled) {
        encrypt(jsonStr,AES_ENCRYPT);                       //加密 
    }
    // Step 2.5: Compress the encrypted JSON string Convert the string to a span of characters
    std::span<char> span{jsonStr};
    // Use the provided compressGZ function to compress the data
    yasio::byte_buffer compressedData = ZipUtils::compressGZ(span);         //压缩数据
    // Step 3: Base64 encode the compressed data 
    return  utils::base64Encode(compressedData.data(), compressedData.size()); //Base64

}

ValueEx& UserDataEx::getValueEx(std::string_view skey) {
   std::string sskey(skey);  
  //AXLOGD("getValueEx skey:{}  this:{}  _values:{} ",sskey,fmt::ptr(this),fmt::ptr(&_values));
  auto it = _values.find(sskey);
  ////AXLOGD("getValueEx skey:{}  this:{}",skey,fmt::ptr(this));
  if (it != _values.end()) {
        //AXLOGD("Found value it:{}",fmt::ptr(static_cast<void*>(&it->second))); // 假设 ValueEx 有一个 get() 成员
    } else {
        //AXLOGD("No value found for key'");
    }
  if (it != _values.end()) {
    // //AXLOGD("getValueEx skey:{} it->second:{}",skey,fmt::ptr(it->second));
    return it->second;
  }
  throw std::runtime_error("Key not found in UserDataEx.");
}

void UserDataEx::setDoubleForKey(std::string_view skey, double value) {
    std::string sskey(skey);  
  //AXLOGD("setDoubleForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  //AXLOGD("setDoubleForKey check value:{}",_values[sskey].asDouble());
  auto aa     = getDoubleForKey(sskey);
  //AXLOGD("getDoubleForKey check value:{}",aa);
  modify();
  //_isModified=true;                                          //是否修改标志
}

void UserDataEx::setFloatForKey(std::string_view skey, float value) {
 std::string sskey(skey);  
  //AXLOGD("setFloatForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  modify();
  //_isModified=true;                                          //是否修改标志
}

void UserDataEx::setTypeValueForKey(std::string_view skey,std::string_view value,u_char ntype){
 std::string sskey(skey);  
 //AXLOGD("setTypeValueForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value,ntype);
  modify();
  //_isModified=true;                                          //是否修改标志
}

void UserDataEx::setStringForKey(std::string_view skey,
                                 std::string_view value) {
    std::string sskey(skey);                                      
  //AXLOGD("setStringForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  modify();
  //_isModified=true;                                          //是否修改标志
}
void  UserDataEx::setStringForKey(std::string skey, std::string value){
  std::string sskey(skey);  
  //AXLOGD("setStringForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  modify();
  //_isModified=true;                                          //是否修改标志
}

void UserDataEx::setBoolForKey(std::string_view skey, bool value) {
  std::string sskey(skey);  
  //AXLOGD("setBoolForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  modify();
  //_isModified=true;                                          //是否修改标志
}

void UserDataEx::setIntegerForKey(std::string_view skey, int value) {
  std::string sskey(skey);  
  //AXLOGD("setIntegerForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  modify();
  //_isModified=true;                                          //是否修改标志
}

void UserDataEx::setUnsignedForKey(std::string_view skey,
                                   unsigned int value) {
  std::string sskey(skey);  
  //AXLOGD("setUnsignedForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  modify();
  //_isModified=true;                                          //是否修改标志
}

void  UserDataEx::setInt64ForKey(std::string_view skey, int64_t value){
  std::string sskey(skey);  
  //AXLOGD("setInt64ForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  modify();
  //_isModified=true;                                          //是否修改标志
};
void UserDataEx::setUnsignedInt64ForKey(std::string_view skey, uint64_t value){
  std::string sskey(skey);  
  //AXLOGD("setUnsignedInt64ForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  modify();
  //_isModified=true;                                          //是否修改标志
};



void UserDataEx::setByteForKey(std::string_view skey, unsigned char value) {
  std::string sskey(skey);  
  //AXLOGD("setByteForKey skey:{} value:{} this:{}",sskey,value,fmt::ptr(this));
  _values[sskey] = ValueEx(value);
  modify();
  //_isModified=true;                                          //是否修改标志
}

double UserDataEx::getDoubleForKey(std::string_view skey) {
  std::string sskey(skey);  
  //AXLOGD("getDoubleForKey skey:{} this:{}",sskey,fmt::ptr(this));
//   auto exd=getValueEx(skey);
//   //AXLOGD("getDoubleForKey skey:{} exd:{}",skey,fmt::ptr(&exd));
//   return exd.asDouble();
// 使用 find 函数检查 "abc" 是否存在
    auto it = _values.find(sskey);
    if (it != _values.end()) {
        // 如果存在，it 指向该元素
        auto valueEx = it->second;
        //AXLOGD("getDoubleForKey skey:{} asdouble:{}",sskey, valueEx.asDouble());
        return valueEx.asDouble();
    } else {
        // 如果不存在，可以通过其他方式处理
        // 例如抛出异常、返回默认值或插入新值
        //AXLOGD("getDoubleForKey skey:{} 没找到",sskey);
        return 0.0;
    }
}

float UserDataEx::getFloatForKey(std::string_view skey) {
  std::string sskey(skey);  
  //AXLOGD("getFloatForKey skey:{} this:{}",sskey,fmt::ptr(this));
  return getValueEx(sskey).asFloat();
}

bool UserDataEx::getBoolForKey(std::string_view skey) {
  std::string sskey(skey);  
  //AXLOGD("getBoolForKey skey:{} this:{}",sskey,fmt::ptr(this));
  return getValueEx(sskey).asBool();
}

std::string UserDataEx::getStringForKey(std::string_view skey) {
  std::string sskey(skey);  
  //AXLOGD("getStringForKey skey:{} this:{}",sskey,fmt::ptr(this));
  return getValueEx(sskey).asString();
}

bool UserDataEx::deleteForKey(std::string_view skey){
    // 使用 erase 函数删除 key，返回被删除元素的数量（0 或 1）
    std::string sskey(skey);  
    auto result = _values.erase(sskey);
        // 如果 result 是 1，说明找到了 key 并成功删除；如果是 0，说明 key 不存在
    return result > 0;
    // return getValueEx(skey).asString();
};
int UserDataEx::getIntegerForKey(std::string_view skey) {
  //AXLOGD("getIntegerForKey skey:{} this:{}",skey,fmt::ptr(this));
  return getValueEx(skey).asInt();
}

unsigned int UserDataEx::getUnsignedForKey(std::string_view skey) {
  //AXLOGD("getUnsignedForKey skey:{} this:{}",skey,fmt::ptr(this));
  return getValueEx(skey).asUInt();
}
int64_t UserDataEx::getInt64ForKey(std::string_view skey) {
  //AXLOGD("getInt64ForKey skey:{} this:{}",skey,fmt::ptr(this));  
  return getValueEx(skey).asInt64();
}

uint64_t UserDataEx::getUnsignedInt64ForKey(std::string_view skey) {
  //AXLOGD("getUnsignedInt64ForKey skey:{} this:{}",skey,fmt::ptr(this));  
  return getValueEx(skey).asUInt64();
}

unsigned char UserDataEx::getByteForKey(std::string_view skey) {
  //AXLOGD("getByteForKey skey:{} this:{}",skey,fmt::ptr(this));  
  return getValueEx(skey).asUChar();
}

void UserDataEx::ud_setkey(std::string& lhs,std::string_view rhs){
  static const size_t keyLen = 16;
  if (!rhs.empty()) {
    lhs.assign(rhs.data(), std::min(rhs.length(), keyLen));
    if (lhs.size() < keyLen)
      lhs.insert(lhs.end(), keyLen - lhs.size(),'\0');  // fill 0, if key insufficient
  } else
    lhs.assign(keyLen, '\0');  
}

void UserDataEx::setEncryptEnabled(bool enabled, std::string_view key, std::string_view iv)
{
    _encryptEnabled = enabled;
    if (_encryptEnabled)
    {
        ud_setkey(_key, key);       //存放到 _key 空间
        ud_setkey(_iv, iv);         //存放到 _iv 空间 
    }
  modify();
  //_isModified=true;                                          //是否修改标志
}

void UserDataEx::encrypt(std::string& inout, int enc) {
    if (inout.size() > 0) {
    AES_KEY aeskey;
    if (AES_set_encrypt_key((const unsigned char*)_key.c_str(), 128, &aeskey) != 0) {
        AXLOGE("Failed to set AES encryption key.");
        return;
    }

    unsigned char iv[16];
    if (_iv.size() > sizeof(iv)) {
        AXLOGW("IV is too long, truncating.");
    }
    memcpy(iv, _iv.data(), std::min(_iv.size(), sizeof(iv)));

    int ignored_num = 0;
    AES_cfb128_encrypt((unsigned char*)inout.data(), 
                           (unsigned char*)inout.data(),
                           inout.size() ,
                           &aeskey, iv, 
                           &ignored_num, enc);
    }
}
void UserDataEx::encrypt(char* inout, size_t size, int enc) {
      if (size > 0) {
        AES_KEY aeskey;
        AES_set_encrypt_key((const unsigned char*)_key.c_str(), 128, &aeskey);

        unsigned char iv[16] = {0};
        memcpy(iv, _iv.c_str(), std::min(sizeof(iv), _iv.size()));

        int ignored_num = 0;
        AES_cfb128_encrypt((unsigned char*)inout, (unsigned char*)inout, size,
                           &aeskey, iv, &ignored_num, enc);
      }
}
//---class function
//--------------------------------------------------------------------------------------------

void UserDataEx::deleteStorage(std::string_view name) 
{
    std::string  rkey;                             //组合主键       
    auto ud =UserDefault::getInstance();
    //AXLOGD("deleteStorage name:{} this:{}",name,fmt::ptr(ud));  
    if (ud){
        for (const auto& pair : _dictionaryemap) {
           //std::cout << "Key: " << pair.first << std::endl;
           auto rkey=std::string(name)+pair.first;
           //AXLOGD("删除 键:{}",rkey);
        //    //AXLOGD("deleteStorage name:{} this:{}",name,fmt::ptr(ud));  
           ud->deleteValueForKey(rkey.c_str());
        }
    }
}
void UserDataEx::saveStorage() 
{
    auto ud =UserDefault::getInstance();        
    //AXLOGD("saveStorage this:{}",fmt::ptr(ud));  
    if (ud){
        for (const auto& pair : _dictionaryemap) {      //遍历所有的 实例 执行存储
           auto rkey=_storagename+std::string(pair.first);
           //AXLOGD("执行 所有函数的 保存:{}",rkey);
           pair.second->saveData();
        }
    }
}

void UserDataEx::setStorageName(std::string_view name) {
  if (!_storagename.empty()){
    deleteStorage(getStorageName());                    //从持久化 删除所有的 数据
  } 
  _storagename =std::string(name);
  saveStorage();                        //保存所有数据到 持久化
}

void UserDataEx::clearAll(){                                   //  清除所有的字典
    deleteStorage(_storagename);                               // 清除存储的数据
    _dictionaryemap.clear();                                   // 删除所有的字典地点 
}
std::string_view UserDataEx::getStorageName() { return _storagename; }

bool UserDataEx::saveData(){
    //从 UserDefault 获得 持久数据加载
    //if (_isModified) {
    auto rkey=_storagename+std::string(_udname);       //组合主键       
    //AXLOGD("saveData  rkey:{}",rkey);  
    if (!rkey.empty()) {
        auto ud =UserDefault::getInstance();
        //AXLOGD("saveData  rkey:{}  ud:{} ",rkey,fmt::ptr(ud));
        if (ud){
            auto svalue =  GenJsonString();             //获得生成字符串 以 复合Key  写入到 UserDefault
            //AXLOGD("saveData  rkey:{}  ud:{}  svalue:{}",rkey,fmt::ptr(ud),svalue);
            ud->setStringForKey(rkey.c_str(),std::string_view(svalue.data()));
            _isModified=false;
            // return true;
        }
    }
    //}
    return true;
}
bool UserDataEx::checkAndSave(){
    //从 UserDefault 获得 持久数据加载
    if (_isModified) {
        return saveData();
    }
    return false;
}

UserDataEx* UserDataEx::GetUserDataEx(std::string_view skey) {
  // Check if the UserDataEx instance already exists in the map
  std::string sskey(skey);    
  auto it = _dictionaryemap.find(sskey);
    //AXLOGD("GetUserDataEx skey:{}  it:{}",skey,fmt::ptr(it));
  if (it != _dictionaryemap.end()) {
    //Return the existing instance
    //AXLOGD("GetUserDataEx skey:{}  it-second.get:{}",skey,fmt::ptr(it->second.get()));
    return it->second.get();
  }
    //Attempt to create and insert a new UserDataEx instance atomically
  auto result = _dictionaryemap.emplace(sskey, std::make_unique<UserDataEx>(sskey));  //带 名字创建的时候，会自动从 持久化存储 获取历史信息
    //AXLOGD("GetUserDataEx skey:{}  result:{}",skey,fmt::ptr(result));
  if (result.second) {
    //AXLOGD("GetUserDataEx skey:{}  result:{}",skey,fmt::ptr(result.first->second.get()));
    return result.first->second.get();
  }
  return nullptr;
}


bool UserDataEx::DeleteUserDataEx(std::string_view skey) {
  std::string sskey(skey);  
  auto it = _dictionaryemap.find(sskey);
    //AXLOGD("DeleteUserDataEx skey:{}  it:{}",skey,fmt::ptr(it));
  if (it != _dictionaryemap.end()) {
    //Erase the UserDataEx instance from the map
    //AXLOGD("DeleteUserDataEx skey:{} find delete",skey);
    deleteStorage(sskey);                                    //从持久删除 一个 字典。
    _dictionaryemap.erase(it);
    return true;
  }
  return false;
}


NS_AX_END
