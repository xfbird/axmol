#include "LogSave.h"

namespace logsv {

LogSave* LogSave::instance = nullptr;  // 定义，不使用 __declspec
std::mutex LogSave::mutex;            // 定义，不使用 __declspec

LogSave* LogSave::GetInstance() {
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr) {
        instance = new LogSave();
    }
    return instance;
}

void LogSave::SaveLog(const std::string& logmsg) {
    _logs.push_back(logmsg);
}

std::string LogSave::GetLog() {
    if (_logs.empty()) {
        return "";
    }
    std::string log = _logs.front();
    _logs.erase(_logs.begin());
    return log;
}

void SaveLogInfo(const std::string& logmsg){
    auto ls=LogSave::GetInstance();
    if  (ls!=nullptr) {
        ls->SaveLog(logmsg);
    } ;
};

LOGSAVE_API std::string GetLogInfo(){
    auto ls=LogSave::GetInstance();
    if  (ls!=nullptr) {
        return ls->GetLog();
    } 
    return "";
};


} // namespace logsv