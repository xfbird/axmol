#include "netClient.h"

#include <string>

#include "Bit6EnDecode.h"
#include "base/Logging.h"
namespace ax
{
namespace gameex
{
NetClient* NetClient::create()
{
    AXLOGD("NetClient::create");
    return new NetClient();
}
void NetClient::destroyInstance()
{
    AXLOGD("NetClient::destroyInstance this:{:12X}", FMT_TOPOINT(this));
    delete this;
}
void NetClient::GetsocketInfo() const
{
    if (!CheckMemoryIntegrity())
    {
        AXLOGD("NetClient::GetsocketInfo this:{:12X} 内存已经被覆盖", FMT_TOPOINT(this));
    }
    AXLOGD("NetClient::GetsocketInfo this:{:12X}", FMT_TOPOINT(this));
    std::shared_lock<std::shared_mutex> lock1(_tcpClientMutex);
    AXLOGD("NetClient::GetsocketInfo  _tcpClientMutex 加锁 成功");
    std::shared_lock<std::shared_mutex> lock2(_transportMutex);
    AXLOGD("NetClient::GetsocketInfo  _transportMutex 加锁 成功");
    std::shared_lock<std::shared_mutex> lock3(_tcpSinkMutex);
    AXLOGD("NetClient::GetsocketInfo  _tcpSinkMutex 加锁 成功");
    if (m_tcpClient)
    {
        AXLOGD("m_tcpClient:{:12X}", FMT_TOPOINT(m_tcpClient.get()));
    }
    else
    {
        AXLOGD("m_tcpClient:isnull");
    }
    if (m_transport)
    {
        AXLOGD("m_transport:{:12X}", FMT_TOPOINT(m_transport));
    }
    else
    {
        AXLOGD("m_transport:isnull");
    }
    if (m_tcpSink)
    {
        AXLOGD("m_tcpSink:{:12X}", FMT_TOPOINT(m_tcpSink));
    }
    else
    {
        AXLOGD("m_tcpSink:isnull");
    }
}
void NetClient::Tick()
{
    // if (IsConnected()) {
#if defined(_WIN32) && defined(_DEBUG)
    if (_CrtCheckMemory() == 0)
    {
        AXLOGE("Heap corruption detected!");
    }
#endif
    GetsocketInfo();
    std::shared_lock<std::shared_mutex> lock(_tcpClientMutex);
    if (m_tcpClient)
    {
        AXLOGD("NetClient::Tick dispatch");
        m_tcpClient->dispatch();
        AXLOGD("NetClient::Tick dispatch return");
    }
}
bool NetClient::CheckMemoryIntegrity() const
{
    for (int i = 0; i < 64; ++i)
    {
        if (PRE_GUARD[i] != POST_GUARD[i])
        {
            AXLOGE("Memory corruption detected at guard block {}", i);
            return false;
        }
    }
    AXLOGD("对象内存哨兵检查通过");
    return true;
}
bool NetClient::Connect(const std::string& ip, unsigned short port)
{
    AXLOGD("NetClient::Connect this:{:12X} ip:{} port:{}", FMT_TOPOINT(this), ip, port);
    std::lock_guard<std::shared_mutex> clientLock(_tcpClientMutex);
    AXLOGD("NetClient::Connect  _tcpClientMutex 加锁 成功");
    // 1. 清理旧连接
    if (m_tcpClient)
    {
        m_tcpClient->stop();
        m_tcpClient->close(0);
    }
    // 创建新连接
    auto newClient = std::make_shared<yasio::io_service>(yasio::io_hostent(ip, port));
    if (!newClient)
    {
        AXLOGE("Failed to create io_service");
        return false;
    }
    // 3. 原子性替换
    m_tcpClient = newClient;
    m_tcpClient->set_option(YOPT_C_UNPACK_PARAMS, 0, 0, 0, 0, 0);
    m_tcpClient->set_option(yasio::YOPT_S_NO_DISPATCH, 0);
    if (!m_tcpClient->open(0, yasio::YCK_TCP_CLIENT))
    {
        AXLOGE("Failed to open TCP client");
        return false;
    }
    AXLOGD("NetClient::Connect  Open ok ");
    {
        std::lock_guard<std::shared_mutex> transportLock(_transportMutex);
        m_transport = nullptr;
    }
    m_tcpClient->start(
        [this](yasio::event_ptr&& ev)
        {
            AXLOGD("NetClient::Connect  event in");
            switch (ev->kind())
            {
                case yasio::YEK_ON_PACKET:
                {
                    auto packet = std::move(ev->packet());
                    std::shared_lock<std::shared_mutex> sinkLock(_tcpSinkMutex);
                    AXLOGD("NetClient::Connect YEK_ON_PACKET _tcpSinkMutex 加锁完成");
                    if (m_tcpSink)
                    {
                        AXLOGD("NetClient::Connect YEK_ON_PACKET 调用 m_tcpSink->OnRecv");
                        m_tcpSink->OnRecv(packet.data(), packet.size());
                    }
                    break;
                }
                case yasio::YEK_ON_OPEN:
                {
                    bool ok = false;
                    if (ev->status() == 0)
                    {
                        std::lock_guard<std::shared_mutex> transportLock(_transportMutex);
                        AXLOGD("NetClient::Connect YEK_ON_OPEN _tcpSinkMutex 加锁完成");
                        m_transport = ev->transport();
                        ok          = true;
                    }
                    std::shared_lock<std::shared_mutex> sinkLock(_tcpSinkMutex);
                    AXLOGD("NetClient::Connect YEK_ON_OPEN _tcpSinkMutex 加锁完成");
                    if (m_tcpSink)
                    {
                        AXLOGD("NetClient::Connect YEK_ON_OPEN 调用 m_tcpSink->OnConnect");
                        m_tcpSink->OnConnect(ok);
                    }
                    break;
                }
                case yasio::YEK_ON_CLOSE:
                {
                    std::shared_lock<std::shared_mutex> sinkLock(_tcpSinkMutex);
                    AXLOGD("NetClient::Connect YEK_ON_CLOSE _tcpSinkMutex 加锁完成");
                    if (m_tcpSink)
                    {
                        AXLOGD("NetClient::Connect YEK_ON_CLOSE m_tcpSink->OnClose");
                        m_tcpSink->OnClose();
                    }
                    break;
                }
                default:
                    break;
            }
        });
    return IsConnected();
}
void NetClient::Close()
{
    std::lock_guard<std::shared_mutex> clientLock(_tcpClientMutex);
    AXLOGD("NetClient::Close  _tcpClientMutex 加锁");
    std::lock_guard<std::shared_mutex> transportLock(_transportMutex);
    AXLOGD("NetClient::Close  _transportMutex 加锁");
    if (m_tcpClient)
    {
        AXLOGD("NetClient::Close 调用 m_tcpClient->close");
        m_tcpClient->stop();
        m_tcpClient->close(0);
    }
    m_transport = nullptr;
}
std::string NetClient::get_ServerKey() const
{
    return _ServerKey;
}
void NetClient::set_ServerKey(const std::string& value)
{
    _ServerKey = value;
}
int NetClient::get_TypeData() const
{
    return _TypeData;
}
void NetClient::set_TypeData(int value)
{
    _TypeData = value;
}
void NetClient::SetTcpSink(NetClientSink* tcpSink)
{
    std::lock_guard<std::shared_mutex> lock(_tcpSinkMutex);
    AXLOGD("NetClient::SetTcpSink  _tcpSinkMutex 加锁 成功  SetTcpSink");
    m_tcpSink = tcpSink;
}
void NetClient::CleanTcpSink()
{
    std::lock_guard<std::shared_mutex> lock(_tcpSinkMutex);
    AXLOGD("NetClient::CleanTcpSink  _tcpSinkMutex 加锁 成功  CleanTcpSink");
    m_tcpSink = nullptr;
}
NetClient::NetClient() : _ServerKey(""), _TypeData(0), m_tcpSink(nullptr), m_transport(nullptr)
{
    AXLOGD("NetClient::NetClient this:{:12X}", FMT_TOPOINT(this));
    if (CheckMemoryIntegrity())
    {
        AXLOGD("NetClient::NetClient this:{:12X} 内存哨兵 创建检查 正常", FMT_TOPOINT(this));
    }
    // AXLOGD("NetClient::NetClient return this address: {:12X}",
    // FMT_TOPOINT(this)); GetsocketInfo();
}
NetClient::~NetClient()
{
    AXLOGD("NetClient::~NetClient set this address: {:12X}", FMT_TOPOINT(this));
    if (CheckMemoryIntegrity())
    {
        AXLOGD("NetClient::~NetClient this:{:12X} 内存哨兵 释放检查 正常", FMT_TOPOINT(this));
    }
    // 1. 标记销毁状态，阻止后续操作
    std::lock_guard<std::shared_mutex> clientLock(_tcpClientMutex);
    AXLOGD("NetClient::~NetClient  _tcpClientMutex 加锁 成功");
    std::lock_guard<std::shared_mutex> transportLock(_transportMutex);
    AXLOGD("NetClient::~NetClient  _transportMutex 加锁 成功");
    // 2. 安全释放资源
    if (m_tcpClient)
    {
        m_tcpClient->stop();
        // 确保所有回调完成
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        m_tcpClient->close(0);
        m_tcpClient.reset();  // 显式释放
    }
    m_transport = nullptr;  // 清除无效指针
    m_tcpSink   = nullptr;
}
void NetClient::SendMsg(NetMessage* nmsg)
{
    AXLOGD("NetClient::SendMsg this:{:12X} m_tcpClient:{:12X}", FMT_TOPOINT(this), FMT_TOPOINT(m_tcpClient.get()));
    if (!IsConnected())
    {
        AXLOGE("Connection is not established. Failed to send message.");
        return;
    }
    nmsg->SetParam4(_TypeData);  // 用 _TypeData 覆盖 数据.
    std::string serializedHeader = nmsg->serializedHeader();
    AXLOGD("NetClient::SendMsg  serializedHeader size:{}", serializedHeader.size());
    std::string EncodeHeader = Encode6BitStr(serializedHeader);
    // std::string serializedData = serializedHeader +
    // std::string(nmsg->GetStrInfo()); AXLOGD("NetClient::SendMsg  serializedData
    // size:{}", serializedData.size());
    std::string Encodebase64Data = EncodeHeader + std::string(nmsg->GetStrInfo());
    // AXLOGD("NetClient::SendMsg  Encodebase64Data size:{}",
    // Encodebase64Data.size());
    AXLOGD("NetClient::SendMsg 编码结果:{}", Encodebase64Data);
    Send(Encodebase64Data.data(), Encodebase64Data.size());
    _TypeData++;
}
int NetClient::Send(const void* data, int size)
{
    std::shared_lock<std::shared_mutex> clientLock(_tcpClientMutex);
    AXLOGD("NetClient::Send  _tcpClientMutex 加锁 成功");
    std::shared_lock<std::shared_mutex> transportLock(_transportMutex);
    AXLOGD("NetClient::Send  _transportMutex 加锁 成功");
    if (!IsConnectedInternal())
    {
        return 0;
    }
    yasio::obstream obs;
    obs.write_bytes(data, size);
    AXLOGD("NetClient::Send write_bytes to yasio::obstrea {}", size);
    return m_tcpClient->write(m_transport, std::move(obs.buffer()), nullptr);
}
bool NetClient::IsConnected(void)
{
    std::shared_lock<std::shared_mutex> clientLock(_tcpClientMutex);
    AXLOGD("NetClient::IsConnected  _tcpClientMutex 加锁 成功");
    std::shared_lock<std::shared_mutex> transportLock(_transportMutex);
    AXLOGD("NetClient::IsConnected  _transportMutex 加锁 成功");
    return IsConnectedInternal();
}
bool NetClient::IsConnectedInternal() const
{
    // 检查指针有效性
    if (!m_tcpClient || !m_transport)
    {
        return false;
    }
    // 检查内存是否已被释放（Debug模式下的DDDDDDDD）
    auto clientPtr = reinterpret_cast<uintptr_t>(m_tcpClient.get());
    if (clientPtr == 0xDDDDDDDDDDDDDDDD)
    {
        return false;
    }
    return m_tcpClient->is_open(m_transport) && m_tcpClient->is_running();
}
}  // namespace gameex
}  // namespace ax
