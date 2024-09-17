#include "base/UserDataEx.h"
#include "openssl/aes.h"
#include "openssl/modes.h"
#include "openssl/rc4.h"
#include <stdexcept>
#include "base/Logging.h"
#include "base/Utils.h"
#include "simdjson/simdjson.h"
#include "fmt/format.h"
#include "base/ZipUtils.h"
#include "base/PaddedString.h"
#include <string_view>
#include <span>
#include "yasio/byte_buffer.hpp"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"


NS_AX_BEGIN
std::string UserDataEx::_storagename;
//std::unordered_map<std::string, std::unique_ptr<UserDataEx>>
DataExMap* UserDataEx::_dataExMaps = nullptr;

UserDataEx::UserDataEx() :
   _dataReady(false),
   _isModified(false),
   _autoSave(false),
   _encryptEnabled(false),
   _udname(""),
   _key(""),
   _iv(""),
   _valueExMap(new ValueExMap()){
	    if (_dataExMaps == nullptr) {
	        _dataExMaps = new DataExMap();
			//std::unordered_map<std::string, std::unique_ptr<UserDataEx>>();
	    }
   }
UserDataEx::UserDataEx(std::string_view name)
{
    _udname=std::string(name);
    getUserDatainfoFromDefault();
}
// std::unordered_map<std::string, std::unique_ptr<ValueEx>>

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
            _valueExMap->clear();
            //AXLOGD("getUserDatainfoFromDefault 3 rkey:{} this:{} 读出为空,初始化", 
            //            rkey,fmt::ptr(this));
            return true;    
        }
    }
    return false;
}

UserDataEx::~UserDataEx() {
    if (_isModified) {
        saveData();
	}
	AX_SAFE_DELETE(_valueExMap);
}

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
        _valueExMap->clear();
        if (svdatas.empty()) {
            return true;
        }
        auto comprData   = utils::base64Decode(svdatas);
        auto  uncomprData = ZipUtils::decompressGZ(std::span{comprData});
        std::string sjdata(reinterpret_cast<const char*>(uncomprData.data()), uncomprData.size());
        // AXLOGD("initFromString Get Str:{}",sjdata);
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
            // AXLOGD("initFromString setTypeValueForKey svname:{}  svvalue:{} etypeRes:{} this:{}", svname, svvalue,etype,fmt::ptr(this));
            // setTypeValueForKey(svname, svvalue, etype);

            //需要调整 为 用 反序列化 进行处理。
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
// 在 UserDataEx.cpp 中实现 GetType 函数
int GetType(const ValueEx& value) {
    if (std::holds_alternative<bool>(value)) return 0;
    if (std::holds_alternative<int>(value)) return 1;
    if (std::holds_alternative<int64_t>(value)) return 2;
    if (std::holds_alternative<float>(value)) return 3;
    if (std::holds_alternative<double>(value)) return 4;
    if (std::holds_alternative<std::string>(value)) return 5;
    return -1; // Should never reach here
}

std::string UserDataEx::Serialize() const {
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    for (const auto& [key, valuePtr] : *_valueExMap) {
        const auto& value = *valuePtr;
        rapidjson::Value jsonKey;
        jsonKey.SetString(key.c_str(), allocator);
        // std::cout << "key: " << key << std::endl;

        rapidjson::Value jsonValue;
        jsonValue.SetObject();

        jsonValue.AddMember("type", rapidjson::Value().SetInt(GetType(value)), allocator);

        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, bool>) {
                jsonValue.AddMember("value", rapidjson::Value().SetBool(val), allocator);
                // std::cout << "value "<< val << std::endl;
            } else if constexpr (std::is_same_v<T, int>) {
                jsonValue.AddMember("value", rapidjson::Value().SetInt(val), allocator);
                // std::cout << "value "<< val << std::endl;

            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonValue.AddMember("value", rapidjson::Value().SetInt64(val), allocator);
                // std::cout << "value "<< val << std::endl;
            } else if constexpr (std::is_same_v<T, float>) {
                jsonValue.AddMember("value", rapidjson::Value().SetFloat(val), allocator);
                // std::cout << "value "<< val << std::endl;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonValue.AddMember("value", rapidjson::Value().SetDouble(val), allocator);
                // std::cout << "value "<< val << std::endl;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonValue.AddMember("value", rapidjson::Value().SetString(val.c_str(), allocator), allocator);
                // std::cout << "value "<< val << std::endl;
            }
        }, value);        
        // std::cout << "value "<< value << std::endl;
        document.AddMember(jsonKey, jsonValue, allocator);
    }

    document.Accept(writer);
    return buffer.GetString();
}

bool UserDataEx::Deserialize(const std::string& jsonStr) {
    rapidjson::Document document;
    document.Parse(jsonStr.c_str());
    if (document.HasParseError()) {
        std::cerr << "JSON parse error: " << document.GetParseError() << std::endl;
        return false;
    }

    _valueExMap->clear();
    for (auto& member : document.GetObj()) {
        std::string key = member.name.GetString();
        const rapidjson::Value& jsonValue = member.value;

        int type = jsonValue["type"].GetInt();
        std::unique_ptr<ValueEx> valueEx;
        switch (type) {
            case 0: valueEx = std::make_unique<ValueEx>(jsonValue["value"].GetBool()); break;
            case 1: valueEx = std::make_unique<ValueEx>(jsonValue["value"].GetInt()); break;
            case 2: valueEx = std::make_unique<ValueEx>(jsonValue["value"].GetInt64()); break;
            case 3: valueEx = std::make_unique<ValueEx>(jsonValue["value"].GetFloat()); break;
            case 4: valueEx = std::make_unique<ValueEx>(jsonValue["value"].GetDouble()); break;
            // case 5: valueEx = std::make_unique<ValueEx>(jsonValue["value"].GetString()); break;
            case 5: valueEx = std::make_unique<ValueEx>(std::string(jsonValue["value"].GetString())); break;
            default: continue;
        }
        _valueExMap->emplace(key, std::move(valueEx));
    }
    return true;
}



void UserDataEx::PrintData() const {
    std::cout << "json: " << Serialize() << std::endl;
}

template <typename T>
void UserDataEx::setForKey(const std::string_view& key, T value) {
    _valueExMap->emplace(std::string(key), std::make_unique<ValueEx>(value));
    modify();
}

template <typename T>
T UserDataEx::getForKey(const std::string_view& key, T def) const {
    std::string keyStr(key);
    auto it = _valueExMap->find(keyStr);
    if (it == _valueExMap->end()) {
        // 如果键不存在，则创建一个新的 ValueEx 对象并初始化为默认值
        _valueExMap->emplace(keyStr, std::make_unique<ValueEx>(def));
        return def;
    }
    try {
        // 如果键存在，尝试获取值
        return std::get<T>(*it->second);
    } catch (const std::bad_variant_access&) {
        // 如果类型不匹配，返回默认值
        return def;
    }
}
void UserDataEx::setDoubleForKey(const std::string_view& key, double value){
    setForKey(key,value);
};
void UserDataEx::setFloatForKey(const std::string_view& key, float value){
    setForKey(key,value);
};
void UserDataEx::setStringForKey(const std::string_view& key, const std::string& value){
    setForKey(key,value);
};

void UserDataEx::setBoolForKey(const std::string_view& key, bool value){
    setForKey(key,value);
};
void UserDataEx::setIntegerForKey(const std::string_view& key, int value){
    setForKey(key,value);
};
void UserDataEx::setInt64ForKey(const std::string_view& key, int64_t value){
    setForKey(key,value);
};

double UserDataEx::getDoubleForKey(const std::string_view& key, double value){
    return getForKey(key,value);
};
float UserDataEx::getFloatForKey(const std::string_view& key, float value){
    return getForKey(key,value);
};
bool UserDataEx::getBoolForKey(const std::string_view& key, bool value){
    return getForKey(key,value);
};

std::string UserDataEx::getStringForKey(const std::string_view& key, const std::string& value){
    return getForKey(key,value);
}    
bool UserDataEx::deleteForKey(std::string_view skey) {
    // 检查 _valueExMap 是否为空以及键是否存在
	std::string sskey(skey);  
    if (_valueExMap != nullptr && 
		_valueExMap->find(sskey) != _valueExMap->end()) 
		{
	        // 删除指定的键
    	    _valueExMap->erase(sskey);
        	return true;
    }
    // 键不存在或 _valueExMap 为空
    return false;
}

int UserDataEx::getIntegerForKey(const std::string_view& key, int value){
    return getForKey(key,value);
};
int64_t UserDataEx::getInt64ForKey(const std::string_view& key, int64_t value){
    return getForKey(key,value);
};


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

void UserDataEx::deleteStorage(std::string_view name) 
{
    std::string  rkey;                             //组合主键       
    auto ud =UserDefault::getInstance();
     //AXLOGD("deleteStorage name:{} this:{}",name,fmt::ptr(ud));  
    if (ud){
         for (const auto& pair : *_dataExMaps) {
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
        for (const auto& pair : *_dataExMaps) {      //遍历所有的 实例 执行存储
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
    _storagename = std::string(name);
  saveStorage();                        //保存所有数据到 持久化
}
void UserDataEx::clearAll(){                                   //  清除所有的字典
    deleteStorage(_storagename);                               // 清除存储的数据
    _dataExMaps->clear();                                       // 删除所有的字典地点 
}
std::string_view UserDataEx::getStorageName() {
    return _storagename;
}
bool UserDataEx::saveData(){
    //从 UserDefault 获得 持久数据加载
    //if (_isModified) {
    auto rkey=_storagename+std::string(_udname);       //组合主键       
    //AXLOGD("saveData  rkey:{}",rkey);  
    if (!rkey.empty()) {
        auto ud =UserDefault::getInstance();
        //AXLOGD("saveData  rkey:{}  ud:{} ",rkey,fmt::ptr(ud));
        if (ud){
            auto svalue =  Serialize();             //获得生成字符串 以 复合Key  写入到 UserDefault
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
UserDataEx* UserDataEx::GetUserDataEx(const std::string_view& key) {
    std::string skey=std::string(key);
    if (_dataExMaps->find(skey) == _dataExMaps->end()) {
        _dataExMaps->emplace(skey, std::make_unique<UserDataEx>(skey));
    }
    return _dataExMaps->at(skey).get();
}
bool UserDataEx::DeleteUserDataEx(const std::string_view& key) {

   if (_dataExMaps != nullptr) {
	   std::string sskey(key);  
	   auto it = _dataExMaps->find(sskey);
       //AXLOGD("DeleteUserDataEx skey:{}  it:{}",skey,fmt::ptr(it));
  	   if (it != _dataExMaps->end()) {
	   //Erase the UserDataEx instance from the map
	   //AXLOGD("DeleteUserDataEx skey:{} find delete",skey);
	   deleteStorage(sskey);                                    //从持久删除 一个 字典。
       _dataExMaps->erase(it);
	   return true;
	  }
    }
	return false;    
}
NS_AX_END