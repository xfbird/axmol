#include "NetMessage.h"
#include "base/Logging.h"
namespace ax
{
namespace gameex
{

// 默认构造函数
NetMessage::NetMessage(){
    memset(&_msgh, 0, sizeof(msgHeader));  // 如果 ainfo 的大小小于消息头的大小，初始化消息头为零 
    strinfo.clear();                       // 清空消息内容
}
// 参数化构造函数
NetMessage::NetMessage(int32_t amsgid,
                       int32_t arecog,
                       int32_t aparam1,
                       int32_t aparam2,
                       int32_t aparam3,
                       int32_t aparam4,
                       const std::string_view& asmsg)
    : _msgh{arecog, amsgid, aparam1, aparam2, aparam3, aparam4}, strinfo(asmsg)
{
    //_msgh.sessionID = 0;
    //_msgh.IsZlib    = 0;
    _msgh.Length = strinfo.length();
    _msgh.nIndex = 0;  // Index
    AXLOGD("NetMessage::NetMessage  this:{}",FMT_TOPOINT(this));
}
NetMessage::NetMessage(const std::string_view& ainfo)
{
    if (ainfo.size() >= sizeof(msgHeader))
    {
        std::memcpy(&_msgh, ainfo.data(), sizeof(msgHeader));  // 复制消息头
        if (ainfo.size() > sizeof(msgHeader))
        {
            strinfo = std::string(ainfo.data() + sizeof(msgHeader), ainfo.size() - sizeof(msgHeader));
            AXLOGD("NetMessage::NetMessage  string this:{}",FMT_TOPOINT(this));
            return;
        }
        strinfo.clear();  // 如果 ainfo 的大小刚好等于消息头的大小，则将 strinfo 设置为空字符串
        AXLOGD("NetMessage::NetMessage  string clear this:{}",FMT_TOPOINT(this));
        return;
    }
    memset(&_msgh, 0, sizeof(msgHeader));  // 如果 ainfo 的大小小于消息头的大小，初始化消息头为零
    AXLOGD("NetMessage::NetMessage mgh clear string  clear this:{}",FMT_TOPOINT(this));
    strinfo.clear();                       // 清空消息内容
}

void NetMessage::Destory(){
    AXLOGD("NetClient::Destory this:{:12X}", FMT_TOPOINT(this));
    delete this;
}
// 析构函数
NetMessage::~NetMessage() {
    AXLOGD("NetClient::~NetMessage this:{:12X}", FMT_TOPOINT(this));
};
msgHeader NetMessage::GetMsgHeader() const
{
    return _msgh;
}
void NetMessage::SetMsgHeader(const msgHeader& header)
{
    _msgh = header;
}
int32_t NetMessage::GetMsgID() const
{
    AXLOGD("NetMessage::GetMsgID  this:{}  msgid:{}",FMT_TOPOINT(this),_msgh.msgid);
    return _msgh.msgid;
}
void NetMessage::SetMsgID(int32_t value)
{
    AXLOGD("NetMessage::SetMsgID  this:{} value:{} msgid:{}",FMT_TOPOINT(this),value,_msgh.msgid);
    _msgh.msgid = value;
}
int32_t NetMessage::GetRecog() const
{
    AXLOGD("NetMessage::GetRecog  this:{}  Recog:{}",FMT_TOPOINT(this),_msgh.recog);
    return _msgh.recog;
}
void NetMessage::SetRecog(int32_t value)
{
    AXLOGD("NetMessage::SetRecog  this:{} value:{} recog:{}",FMT_TOPOINT(this),value,_msgh.recog);
    _msgh.recog = value;
}
int32_t NetMessage::GetParam1() const
{
    return _msgh.param1;
}
void NetMessage::SetParam1(int32_t value)
{
    _msgh.param1 = value;
}
int32_t NetMessage::GetParam2() const
{
    return _msgh.param2;
}
void NetMessage::SetParam2(int32_t value)
{
    _msgh.param2 = value;
}
int32_t NetMessage::GetParam3() const
{
    return _msgh.param3;
}
void NetMessage::SetParam3(int32_t value)
{
    _msgh.param3 = value;
}
int32_t NetMessage::GetParam4() const
{
    return _msgh.param4;
}
void NetMessage::SetParam4(int32_t value)
{
    _msgh.param4 = value;
}


int32_t NetMessage::GetLength() const
{
    return _msgh.Length;
}
int32_t NetMessage::GetIsZlib() const
{
    return _msgh.nIsZlib;
}
int32_t NetMessage::GetIndex() const
{
    return _msgh.nIndex;
}

std::string_view NetMessage::GetStrInfo() const
{
    return std::string_view(strinfo);
}
void NetMessage::SetStrInfo(const std::string_view& info)
{
    strinfo = std::string(info);  // 将 std::string_view 转换为 std::string
    _msgh.Length = info.size();
}
std::string NetMessage::serializedHeader() const
{
    AXLOGD("NetClient::SendMsg serializedHeader  sizeof(msgHeader):{}", sizeof(msgHeader));
    return std::string(reinterpret_cast<const char*>(&_msgh), sizeof(msgHeader));
}
}  // namespace gameex
}  // namespace ax