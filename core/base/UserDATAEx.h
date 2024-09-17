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
#ifndef _USERDATAEX_H
#define _USERDATAEX_H

#include "platform/PlatformMacros.h"
//#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include "base/UserDefault.h"
#include "base/Object.h"
#include <variant>
#include <memory>

NS_AX_BEGIN
class UserDataEx;  // Forward declaration for UserDataEx
typedef std::variant<bool, int, int64_t, float, double, std::string> ValueEx;
typedef std::unordered_map<std::string, std::unique_ptr<ValueEx>> ValueExMap;
typedef std::unordered_map<std::string, std::unique_ptr<UserDataEx>>  DataExMap;

class AX_DLL UserDataEx : public Object
{
//----------------------------------------------------------------------------------------------------
//标准成员 部分    
public:
    UserDataEx();
    explicit UserDataEx(std::string_view sname);

    // UserDataEx::UserDataEx(std::string name):
    //     _dataReady(false),
    //     _isModified(false),
    //     _autoSave(false),
    //     _encryptEnabled(false),
	//     name(std::move(name)), 
    //     _key(""),
    //     _iv(""),
	//     _valueExMap(new ValueExMap()){};

    ~UserDataEx();

    void PrintData() const;
    // 公开的设置函数
    void setDoubleForKey(const std::string_view& key, double value);
	void setFloatForKey(const std::string_view& key, float value);
	void setBoolForKey(const std::string_view& key, bool value);
	void setInt64ForKey(const std::string_view& key, int64_t value);
	void setIntegerForKey(const std::string_view& key, int value);
    void setStringForKey(const std::string_view& key, const std::string& value);
    double getDoubleForKey(const std::string_view& key, double value=0.0);
	float getFloatForKey(const std::string_view& key, float value=0.0);
	bool getBoolForKey(const std::string_view& key, bool value=false);
	int getIntegerForKey(const std::string_view& key, int value=0);
    int64_t getInt64ForKey(const std::string_view& key, int64_t value=0);
    std::string getStringForKey(const std::string_view& key, const std::string& value="");
    bool deleteForKey(std::string_view skey);
    void setEncryptEnabled(bool enabled, std::string_view key, std::string_view iv);
    bool getUserDatainfoFromDefault(); //从 User Default 获得一个字典的所有信息       
    bool checkAndSave();
    void setAutoSave(bool enabled){_autoSave=enabled;}
    // void setEncryptEnabled(bool enabled, std::string_view key, std::string_view iv);
private:
    //私有 函数
    bool initFromString(std::string_view rhs);                //从一个字符串 恢复 一个字典表  
    virtual void encrypt(char* inout, size_t size, int enc);  // 加密 解密函数
    void encrypt(std::string& inout, int enc);                // 加密 解密函数
    void modify();
    bool saveData();                                          //保存数据 到 Default          
    void ud_setkey(std::string& lhs,std::string_view rhs);    //设置 加密字符串  
    template <typename T>
    void setForKey(const std::string_view& key, T value);
    template <typename T>
    T getForKey(const std::string_view& key, T def) const;
    std::string Serialize() const;                      //序列化 
    bool Deserialize(const std::string& jsonStr);       //反序列化
private:
    ValueExMap* _valueExMap;                            //数据集的值表
    bool _isModified;                                   //是否修改标志
    bool _dataReady;                                    //是否 已经加载数据
    bool _autoSave;                                     //是否 自动保存
    std::string _udname;                                   //数据集名称
    std::string _key;                                         //加密 Key  
    std::string _iv;                                          //加密 iv  
    bool _encryptEnabled = false;                             //是否 加密标志
    // 模板函数声明
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//类成员 部分    
public:

    static UserDataEx* GetUserDataEx(const std::string_view& key);
    static bool DeleteUserDataEx(const std::string_view& key);
    static void setStorageName(std::string_view name);
    static std::string_view getStorageName();

    static void deleteStorage(std::string_view name);         //  
    static void clearAll();                                   //  清除所有的字典
private:    
    static void saveStorage();	
private:
    static std::string _storagename;
    static std::unordered_map<std::string, std::unique_ptr<UserDataEx>>* _dataExMaps;
};
NS_AX_END
#endif // USERDATAEX_H