
/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "base/Logging.h"
#include <algorithm> // 包含 std::min 和 std::max 函数
#include "yasio/utils.hpp"
#include "fmt/color.h"

#if defined(_WIN32)
#    include "ntcvt/ntcvt.hpp"
#endif

#if defined(__ANDROID__)
#    include <android/log.h>
#endif

namespace ax
{

#if defined(_AX_DEBUG) && _AX_DEBUG == 1
static LogLevel s_logLevel = LogLevel::Debug;
#else
static LogLevel s_logLevel = LogLevel::Info;
#endif

static LogFmtFlag s_logFmtFlags = LogFmtFlag::Null;
static ILogOutput* s_logOutput  = nullptr;

AX_API void setLogLevel(LogLevel level)
{
    s_logLevel = level;
}

AX_API LogLevel getLogLevel()
{
    return s_logLevel;
}

// AX_API std::string stdstrUpcase(const std::string& str)
// {
// // std::string toUpperCase(const std::string& str) {
//     std::string result = str; // 创建原始字符串的副本
//     std::transform(result.begin(), result.end(), result.begin(),
//                    [](unsigned char c) { return std::toupper(c); });
//     return result;
// }

AX_API void setLogFmtFlag(LogFmtFlag flags)
{
    s_logFmtFlags = flags;
}

AX_API void setLogOutput(ILogOutput* output)
{
    s_logOutput = output;
}

AX_API LogItem& preprocessLog(LogItem&& item,const char* fname,int fline,const char* callinfo,int mod)
{
    if (s_logFmtFlags != LogFmtFlag::Null)
    {
        #if defined(_WIN32)
        #    define xmol_getpid()       (uintptr_t)::GetCurrentProcessId()
        #    define xmol_gettid()       (uintptr_t)::GetCurrentThreadId()
        #    define localtime_r(utc, t) ::localtime_s(t, utc)
        #else
        #    define xmol_getpid() (uintptr_t)::getpid()
        #    define xmol_gettid() (uintptr_t)::pthread_self()
        #endif
        auto wptr              = item.prefix_buffer_;
        const auto buffer_size = sizeof(item.prefix_buffer_);
        auto& prefix_size      = item.prefix_size_;
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::Level | LogFmtFlag::Colored))
        {
            std::string_view levelName;
            switch (item.level_)
            {
                case LogLevel::Trace:
                    levelName = "V/"sv;
                    break;
                case LogLevel::Debug:
                    levelName = "D/"sv;
                    break;
                case LogLevel::Info:
                    levelName = "I/"sv;
                    break;
                case LogLevel::Warn:
                    levelName = "W/"sv;
                    break;
                case LogLevel::Error:
                    levelName = "E/"sv;
                    break;
                case LogLevel::Silent:
                    levelName = "S/"sv;
                    break;
                default:
                    levelName = "?/"sv;
            }

            #if !defined(__APPLE__) && !defined(__ANDROID__)
                if (bitmask::any(s_logFmtFlags, LogFmtFlag::Colored))
                {
                    constexpr auto colorCodeOfLevel = [](LogLevel level) -> std::string_view {
                        switch (level)
                        {
                        case LogLevel::Verbose:
                            return "\x1b[37m"sv;
                        case LogLevel::Debug:
                            return "\x1b[36m"sv;
                        case LogLevel::Info:
                            return "\x1b[92m"sv;
                        case LogLevel::Warn:
                            return "\x1b[33m"sv;
                        case LogLevel::Error:
                            return "\x1b[31m"sv;
                        default:
                            return std::string_view{};
                        }
                    };

                    auto colorCode = colorCodeOfLevel(item.level_);
                    if (!colorCode.empty())
                    {
                        item.writePrefix(colorCode);
                        item.has_style_ = true;
                    }
                }
            #endif
            item.writePrefix(levelName);
        }
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::TimeStamp) || bitmask::any(s_logFmtFlags, LogFmtFlag::Date))
        {
            struct tm ts = {0};
            auto tv_msec = yasio::clock<yasio::system_clock_t>();
            auto tv_sec  = static_cast<time_t>(tv_msec / std::milli::den);
            localtime_r(&tv_sec, &ts);
            bool istimes=bitmask::any(s_logFmtFlags, LogFmtFlag::TimeStamp);
            bool isdate=bitmask::any(s_logFmtFlags, LogFmtFlag::Date);
            if (istimes && isdate) { //年月日 时间都显示
                prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,
                                            "[{}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:03d}]", ts.tm_year + 1900,
                                            ts.tm_mon + 1, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec,
                                            static_cast<int>(tv_msec % std::milli::den)).size;

            } else {
               if  (istimes) {  //仅显示时间
                    prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,
                                            "[{:02d}:{:02d}:{:02d}.{:03d}]",ts.tm_hour, ts.tm_min, ts.tm_sec,
                                            static_cast<int>(tv_msec % std::milli::den)).size;
               } else
               {
                if  (isdate) {  //仅显示 日期
                    prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,
                                            "[{}-{:02d}-{:02d}]",ts.tm_year + 1900,ts.tm_mon + 1, ts.tm_mday).size;
                }
               }


            }
        }
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::ProcessId))
            prefix_size +=
                fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size, "[P:{:x}]", xmol_getpid()).size;
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::ThreadId))
            prefix_size +=
                fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size, "[T:{:x}]", xmol_gettid()).size;
        // 检查是否需要添加文件名到日志前缀
        bool iswide = bitmask::any(s_logFmtFlags, LogFmtFlag::WideName);
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::SourceFn) && fname != nullptr) {
            // 计算文件名字符串的长度
            size_t fname_len = strlen(fname);
            // 截取文件名的最后25个字符，如果长度不足25，则在前面补空格
            size_t nmax=23;
            if (iswide) 
            {
              nmax =30;
            }
            size_t nlen=nmax;
            size_t nstart=0;
            if (fname_len<nlen) {       //如果 文件名长度 比 nlen 小 那么长度 按照 文件名长度 设置
                nlen=fname_len;
            } else{
                if (fname_len - nlen>2) {
                    nstart=fname_len - nlen-2;
                }
            }            
            // std::string fname_view(fname + fname_len - nlen,nlen);
            std::string fname_view(fname + nstart,nlen);
            // if (fname_view.length()>nmax) {                           //如果超过 22 字符 删除 尾部 2个字符
            //    fname_view.erase(fname_view.length() - 2); 
            // }
            while (fname_view.length() <nmax) 
                {
                    fname_view = " "+fname_view; // 在前面补空格
                }
            // 格式化文件名并添加到日志前缀
            if (iswide) {
                prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"[{:.35}]", fname_view).size;
            } else 
            {
                prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"[{:.25}]", fname_view).size;
            }
            
        };
        switch (mod) {
            case 0:
                // CPP and C
                prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"{}","C").size;
                break;
            case 1:
                // Lua have Line
                prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"{}","L").size;
                break;
            case 2:
                // Lua Print
                prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"{}","P").size;
                break;
            case 3:
                // Lua Release Print
                prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"{}","R").size;
                break;
            // ...
            default:
                // Orthers 
                prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"{}","X").size;
                break;
        } ;
        // 检查是否需要添加行号到日志前缀
        if (bitmask::any(s_logFmtFlags, LogFmtFlag::SourceFl)&& fline >=0) {
            if (fline>10000) {
              prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"{:5d}]", fline).size;
            } else {
              prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"[{:4d}]", fline).size;
            }
            
        };
        if (callinfo != nullptr) {
            prefix_size += fmt::format_to_n(wptr + prefix_size, buffer_size - prefix_size,"{}]", callinfo).size;
        };

    }
    return item;
}

AX_DLL void outputLog(LogItem& item, const char* tag)
{
    if (!s_logOutput) writeLog(item, tag);
    else s_logOutput->write(item, tag);
}

AX_DLL void writeLog(LogItem& item, const char* tag)
{
#if defined(__ANDROID__)
    int prio;
    switch (item.level_)
    {
    case LogLevel::Info:
        prio = ANDROID_LOG_INFO;
        break;
    case LogLevel::Warn:
        prio = ANDROID_LOG_WARN;
        break;
    case LogLevel::Error:
        prio = ANDROID_LOG_ERROR;
        break;
    default:
        prio = ANDROID_LOG_DEBUG;
    }
    struct trim_one_eol
    {
        explicit trim_one_eol(std::string& v) : value(v)
        {
            trimed = !v.empty() && v.back() == '\n';
            if (trimed)
                value.back() = '\0';
        }
        ~trim_one_eol()
        {
            if (trimed)
                value.back() = '\n';
        }
        operator const char* const() const { return value.c_str(); }
        std::string& value;
        bool trimed{false};
    };
    __android_log_print(prio, tag, "%s",
                        static_cast<const char*>(trim_one_eol{item.qualified_message_} + item.prefix_size_));
#else
    AX_UNUSED_PARAM(tag);
#    if defined(_WIN32)
    if (::IsDebuggerPresent())
        OutputDebugStringW(ntcvt::from_chars(item.message()).c_str());
#    endif

        // write normal color text to console
#    if defined(_WIN32)
    auto hStdout = ::GetStdHandle(item.level_ != LogLevel::Error ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    if (hStdout)
    {
        // print to console if possible
        // since we use win32 API, the ::fflush call doesn't required.
        // see: https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-flushfilebuffers#return-value
        ::WriteFile(hStdout, item.qualified_message_.c_str(), static_cast<DWORD>(item.qualified_message_.size()),
                    nullptr, nullptr);
    }
#    else
    // Linux, Mac, iOS, etc
    auto outfp = item.level_ != LogLevel::Error ? stdout : stderr;
    auto outfd = ::fileno(outfp);
    ::fflush(outfp);
    ::write(outfd, item.qualified_message_.c_str(), item.qualified_message_.size());
#    endif
#endif
}
#ifndef AX_CORE_PROFILE
AX_API void print(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    auto message = StringUtils::vformat(format, args);
    va_end(args);

    if (!message.empty())
        outputLog(LogItem::vformat(FMT_COMPILE("{}{}\n"), preprocessLog(LogItem{LogLevel::Silent},__FILE__,__LINE__,nullptr,0), message),
                  "axmol debug info");
}
#endif
}
