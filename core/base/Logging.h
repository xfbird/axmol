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

#pragma once

#include "base/bitmask.h"
#include "platform/PlatformMacros.h"

#include "fmt/compile.h"

namespace ax
{

enum class LogLevel
{
    Verbose,
    Trace = Verbose,
    Debug,
    Info,
    Warn,
    Error,
    Silent /* only for setLogLevel(); must be last */
};

enum class LogFmtFlag
{
    Null,
    Level     = 1,
    TimeStamp = 1 << 1,
    ProcessId = 1 << 2,
    ThreadId  = 1 << 3,
    Colored   = 1 << 4,
    SourceFn  = 1 << 5,                //源码文件名
    SourceFl  = 1 << 6,                //源码行号
    Full      = Level | TimeStamp | ProcessId | ThreadId | Colored|SourceFn|SourceFl,
};
AX_ENABLE_BITMASK_OPS(LogFmtFlag);

class LogItem
{
    // friend AX_API LogItem& preprocessLog(LogItem&& logItem);
    friend AX_API LogItem& preprocessLog(LogItem&& item,const char* fname,size_t fline);
    friend AX_API void writeLog(LogItem& item, const char* tag);

public:
    static constexpr auto COLOR_PREFIX_SIZE    = 5;                      // \x1b[00m
    static constexpr auto COLOR_QUALIFIER_SIZE = COLOR_PREFIX_SIZE + 3;  // \x1b[m

    explicit LogItem(LogLevel lvl) : level_(lvl) {}
    LogItem(const LogItem&) = delete;

    LogLevel level() const { return level_; }

    std::string_view message() const
    {
        return has_style_ ? std::string_view{qualified_message_.data() + COLOR_PREFIX_SIZE,
                                             qualified_message_.size() - COLOR_QUALIFIER_SIZE}
                          : std::string_view{qualified_message_};
    }

    template <typename _FmtType, typename... _Types>
    inline static LogItem& vformat(_FmtType&& fmt, LogItem& item, _Types&&... args)
    {
        item.qualified_message_ =
            fmt::format(std::forward<_FmtType>(fmt), std::string_view{item.prefix_buffer_, item.prefix_size_},
                        std::forward<_Types>(args)...);

        item.qualifier_size_ = item.prefix_size_;

        auto old_size = item.qualified_message_.size();
        if (item.has_style_)
            item.qualified_message_.append("\x1b[m"sv);
        item.qualifier_size_ += (item.qualified_message_.size() - old_size);
        return item;
    }

private:
    void writePrefix(std::string_view data)
    {
        memcpy(prefix_buffer_ + prefix_size_, data.data(), data.size());
        prefix_size_ += data.size();
    }
    LogLevel level_;
    bool has_style_{false};
    size_t prefix_size_{0};     // \x1b[00mD/[2024-02-29 00:00:00.123][PID:][TID:]
    size_t qualifier_size_{0};  // prefix_size_ + \x1b[m (optional) + \n
    std::string qualified_message_;
    char prefix_buffer_[128];
};

class ILogOutput
{
public:
    virtual ~ILogOutput() {}
    virtual void write(LogItem& item, const char* tag) = 0;
};

/* @brief control log level */
AX_API void setLogLevel(LogLevel level);
AX_API LogLevel getLogLevel();

/* @brief control log prefix format */
AX_API void setLogFmtFlag(LogFmtFlag flags);

/* @brief set log output */
AX_API void setLogOutput(ILogOutput* output);

/* @brief internal use */
AX_API LogItem& preprocessLog(LogItem&& logItem,const char* fname=nullptr,size_t fline=0);

/* @brief internal use */
AX_API void outputLog(LogItem& item, const char* tag);
AX_API void writeLog(LogItem& item, const char* tag);

// inline std::string _line_get_lstr(const std::string& str,size_t dsize=20) {
//     std::string result;
//     // 取得字符串的长度
//     size_t length = str.length();
//     // 如果字符串长度小于20，则在result中补足空格
//     if (length < dsize) {
//         result.resize(dsize, ' '); // 左边补空格到20个字符长度
//     }
//     // 从字符串的倒数第20个字符开始截取，如果不足20个字符，则取全部
//     size_t start_pos = std::max(static_cast<size_t>(0), length - dsize);
//     result = str.substr(start_pos, dsize);
//     // 如果原始字符串长度小于20，将result前面补足空格
//     if (length < dsize) {
//         result.insert(0, 20 - length, ' ');
//     }
//     return result;
// }
// AX_API char* _line_get_lstr(const char sffilen[], int sfline, const char* tag,size_t dsize=20) {
//     // 计算文件名长度
//     size_t sffilen_len = strlen(sffilen);
//     // 确定需要截取的字符串长度
//     size_t extract_len = sffilen_len < dsize ? sffilen_len : dsize;
    
//     // 创建结果字符串，预留足够的空间
//     std::string result;
//     result.reserve(dsize + 12 + strlen(tag) + 1); // 20个字符的空间 + 行号的字符串长度 + 标签 + 终止符

//     // 截取文件名并补空格
//     result = std::string(sffilen + sffilen_len - extract_len, extract_len);
//     while (result.length() < dsize) {
//         result = " " + result; // 在左边补空格
//     }

//     // 将行号转换为字符串并添加到结果中
//     result += std::to_string(sfline);

//     // 添加标签
//     result += tag;

//     // 将std::string转换为char*并返回
//     char* result_cstr = new char[result.length() + 1];
//     strcpy(result_cstr, result.c_str());
//     return result_cstr;
// }


template <typename _FmtType, typename... _Types>
inline void printLogT(_FmtType&& fmt, LogItem& item, _Types&&... args)
// inline void printLogT(const char* sffilen,int sfline,_FmtType&& fmt, LogItem& item, _Types&&... args)
{
    // const char* tag=_line_get_lstr(sffilen,sfline,"axmol");
    if (item.level() >= getLogLevel())
        outputLog(LogItem::vformat(std::forward<_FmtType>(fmt), item, std::forward<_Types>(args)...),"axmol");
        // tag);
}
// __FILE__, __LINE__, SPDLOG_FUNCTION
// ax::printLogT(FMT_COMPILE("{}-{}@{}|" fmtOrMsg "\n"), ax::preprocessLog(ax::LogItem{level}),__FILE__, __LINE__, ##__VA_ARGS__)
#define AXLOG_WITH_LEVEL(level,fmtOrMsg, ...) \
    ax::printLogT(FMT_COMPILE("{}" fmtOrMsg "\n"), ax::preprocessLog(ax::LogItem{level},__FILE__, __LINE__), ##__VA_ARGS__)
    // ax::printLogT(__FILE__, __LINE__,FMT_COMPILE("{}" fmtOrMsg "\n"),ax::preprocessLog(ax::LogItem{level}), ##__VA_ARGS__)
    // ax::printLogT(__FILE__,__LINE__,FMT_COMPILE("{}-",fmtOrMsg "\n"), ax::preprocessLog(ax::LogItem{level}),##__VA_ARGS__)

// FMT_COMPILE(s) FMT_STRING_IMPL(s, fmt::compiled_string, explicit)


#if defined(_AX_DEBUG) && _AX_DEBUG > 0
#    define AXLOGV(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Verbose, fmtOrMsg, ##__VA_ARGS__)
#    define AXLOGD(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Debug, fmtOrMsg, ##__VA_ARGS__)
#else
#    define AXLOGV(...) \
        do              \
        {               \
        } while (0)
#    define AXLOGD(...) \
        do              \
        {               \
        } while (0)
#endif

#define AXLOGI(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Info, fmtOrMsg, ##__VA_ARGS__)
#define AXLOGW(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Warn, fmtOrMsg, ##__VA_ARGS__)
#define AXLOGE(fmtOrMsg, ...) AXLOG_WITH_LEVEL(ax::LogLevel::Error, fmtOrMsg, ##__VA_ARGS__)

#define AXLOGT AXLOGV

#ifndef AX_CORE_PROFILE
/**
 @brief Output Debug message.
 */
/* AX_DEPRECATED(2.1)*/ AX_API void print(const char* format, ...) AX_FORMAT_PRINTF(1, 2);  // use AXLOGD instead
#endif
}
