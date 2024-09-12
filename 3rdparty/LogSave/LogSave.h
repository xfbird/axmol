#ifndef LOGSAVE_H
#define LOGSAVE_H

#include <string>
#include <mutex>
#include <vector>

// 导出宏，用于控制符号的导出
// #ifdef LOGSAVE_EXPORTS
#define LOGSAVE_API __declspec(dllexport)
// #else
// #define LOGSAVE_API __declspec(dllimport)
// #endif

namespace logsv {

class LogSave {
private:
    static LogSave* instance;  // 声明
    static std::mutex mutex;   // 声明
    std::vector<std::string> _logs;

    LogSave() = default;
    ~LogSave() = default;

    LogSave(const LogSave&) = delete;
    LogSave& operator=(const LogSave&) = delete;

public:
    static LogSave* GetInstance();
    void SaveLog(const std::string& logmsg);
    std::string GetLog();
};

LOGSAVE_API void SaveLogInfo(const std::string& logmsg);
LOGSAVE_API std::string GetLogInfo();

} // namespace logsv

#endif // LOGSAVE_H