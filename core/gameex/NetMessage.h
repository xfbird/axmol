#ifndef AXNETMESSAGE_H
#define AXNETMESSAGE_H
#include <cstdint>  // int32_t
#include <string>   // std::string
#include "base/Object.h"
#include "platform/PlatformMacros.h"  // 引入axmol平台判定宏
namespace ax
{
    namespace gameex
    {
    
struct msgHeader
{
    int32_t recog;    // Recog
    int32_t msgid;    // Ident
    int32_t param1;   // Param
    int32_t param2;   // Tag
    int32_t param3;   // Series
    int32_t param4;   // nSessionID
    int32_t nIsZlib;  // nisZlib
    int32_t Length;   // Length
    int32_t nIndex;   // Index
};
class NetMessage : public Object
{
public:
    // 默认构造函数
    NetMessage();
    // 参数化构造函数
    NetMessage(int32_t amsgid,
               int32_t arecog,
               int32_t aparam1,
               int32_t aparam2,
               int32_t aparam3,
               int32_t aparam4,
               const std::string_view& asmsg);
    NetMessage(const std::string_view& ainfo);
    // 析构函数
    ~NetMessage();
    // 消息头的 Get 和 Set 方法
    msgHeader GetMsgHeader() const;
    void SetMsgHeader(const msgHeader& header);
    int32_t GetMsgID() const;
    void SetMsgID(int32_t value);
    int32_t GetRecog() const;
    void SetRecog(int32_t value);
    int32_t GetParam1() const;
    void SetParam1(int32_t value);
    int32_t GetParam2() const;
    void SetParam2(int32_t value);
    int32_t GetParam3() const;
    void SetParam3(int32_t value);
    // 获取和设置 param4 的值
    int32_t GetParam4() const;
    void SetParam4(int32_t value);
    // 消息内容的 Get 和 Set 方法
    std::string_view GetStrInfo() const;
    void SetStrInfo(const std::string_view& info);
    std::string serializedHeader() const;

private:
    msgHeader _msgh;      // 消息头
    std::string strinfo;  // 消息内容
};
}
}  // namespace ax
#endif  // AXNETMESSAGE_H