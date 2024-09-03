/*
    重新来实现 UserDataEx 目的 对外暴露 UserDataEx 类。
    可以 设置公共的引导头 。
    以 Key 可以获得一个 UserDataEx 实例，该实例 里面包含 多个 KV 键值对。
    每一个  UserDataEx 可以独立 请求持久化保存。
    UserDataEx 的每一个 KV 键值对 包含一个默认的 数据类型。会自动存储在 持久化。
    每一个 UserDataEx 可以 独立选择 是否 加密。如果选择加密，那么其 所有的数据 会 加密后存放。
    最终 数据的 持久化，会 存放到 UserDefault 里面作为 一个Key 存在。
    此Key 以 UserDataEx 定义的公共名字存储

    内部考虑。
    主Key 就是 静态变量 _storagename 作为 每一个 字典Key 的前置。

    有一个 map 记录 每一个字典 UserDataEx 
    记录到  UserDefault 里面 每一个 字典 有两个条目 ：
     _storagename_dictname    
     _storagename_dictname_eE = 0/1   Bool  表示是否加密
     如果存在加密 那么增加两个 Key 
     _storagename_dictname_ek = string  加密密钥
     _storagename_dictname_ev = string  加密Key

    每一个 UserDefault 有一个json 存储所有的 KVT 的值。最后可以转换为一个字符串。
    每一个 UserDefault 有一个方法 write  表示需要把 变化的数据进行存储。会触发 整个 UserDefault 数据 转换为字符串并输出到 UserDataEx 存储。
*/
#pragma once
#include "platform/PlatformMacros.h"
#include <string>
#include <string_view>
#include <unordered_map>
// #include "mio/mio.hpp"
#include "base/UserDefault.h"
// #include "yasio/string_view.hpp"
#include "base/UserDATAEx.h"
#include "base/Object.h"
#include "base/ValueEx.h"
#include <memory>

NS_AX_BEGIN

class UserDataEx;  // Forward declaration for UserDataEx

typedef std::unordered_map<std::string, ValueEx> ValueExMap;
typedef std::unordered_map<std::string, std::unique_ptr<UserDataEx>> DataExMap;

class AX_DLL UserDataEx : public Object
{
public:
    UserDataEx();
    explicit UserDataEx(std::string_view sname);
    ~UserDataEx();

    // Methods to get or set specific types of values
    //---某个字典的 设置函数---------------------------------------------------------------
    void setDoubleForKey(std::string_view skey, double value);
    void setFloatForKey(std::string_view skey, float value);
    void setBoolForKey(std::string_view skey, bool value);
    void setInt64ForKey(std::string_view skey, int64_t value);
    void setUnsignedInt64ForKey(std::string_view skey, uint64_t value);
    void setIntegerForKey(std::string_view skey, int value);
    void setUnsignedForKey(std::string_view skey, unsigned int value);
    void setByteForKey(std::string_view skey, unsigned char value);
    void setStringForKey(std::string_view skey, std::string_view value);    
    void setStringForKey(std::string skey, std::string value);    
    //---某个字典的 取出函数---------------------------------------------------------------
    double getDoubleForKey(std::string_view skey);
    float getFloatForKey(std::string_view skey);
    bool getBoolForKey(std::string_view skey);
    int getIntegerForKey(std::string_view skey);
    unsigned int getUnsignedForKey(std::string_view skey);
    int64_t getInt64ForKey(std::string_view skey);
    uint64_t getUnsignedInt64ForKey(std::string_view skey);

    unsigned char getByteForKey(std::string_view skey);    
    std::string getStringForKey(std::string_view skey,std::string_view defstr="");
    bool deleteForKey(std::string_view skey);
    
    void setTypeValueForKey(std::string_view skey,std::string_view value,u_char ntype);
    void setEncryptEnabled(bool enabled, std::string_view key, std::string_view iv);
    bool getUserDatainfoFromDefault(); //从 User Default 获得一个字典的所有信息       
    bool checkAndSave();

    // bool setAutoSave(bool enable){_autoSave=enable }
    void setAutoSave(bool enabled){_autoSave=enabled;}

private:
    //私有 函数
    bool initFromString(std::string_view rhs);                //从一个字符串 恢复 一个字典表  
    virtual void encrypt(char* inout, size_t size, int enc);  // 加密 解密函数
    void encrypt(std::string& inout, int enc);                // 加密 解密函数
    void modify();
    bool saveData();                                          //保存数据 到 Default          

private:
    ValueEx& getValueEx(std::string_view skey);
    ValueExMap _values;
    bool _isModified;                                          //是否修改标志
    bool _dataReady;                                           //是否 已经加载数据
    bool _autoSave;                                            //是否 自动保存
    // encrpyt args
    std::string _udname;                                      //自身的名字
    std::string _key;                                         //加密 Key  
    std::string _iv;                                          //加密 iv  
    bool _encryptEnabled = false;                             //是否 加密标志

    std::string GenJsonString();                              //生成一个字典 包含加密信息的字符串
    std::string GenJsonDictString();                           //生成 一个字典的 数据  
    void ud_setkey(std::string& lhs,std::string_view rhs);    //设置 加密字符串  
    // void ud_write_v_s(yasio::obstream& obs, const cxx17::string_view value);
    //静态部分
    //-------------------------------------------------------------------------
public:
    //---获得某个字典对象实例---------------------------------------------------------------
    static UserDataEx* GetUserDataEx(std::string_view skey);  // 获得一个字典
    static bool DeleteUserDataEx(std::string_view skey);      // 删除一个字典
    //---设置 字典组 的存储文件标识---------------------------------------------------------------
    static void setStorageName(std::string_view name);        // 设置存储文件名
    static std::string_view getStorageName();                  
    static void deleteStorage(std::string_view name);         //  
    static void clearAll();                                   //  清除所有的字典
private:    
    static void saveStorage();
private:    
    static std::string _storagename;                    // UserDefault 主键名 包含连字符_ 
    static DataExMap  _dictionaryemap;                       // 字典表表
};

NS_AX_END