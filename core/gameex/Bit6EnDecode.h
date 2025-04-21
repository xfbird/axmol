
#ifndef GMXXEXT_ENCODE6BITSTR_H
#define GMXXEXT_ENCODE6BITSTR_H
#include <string>
#include <string_view>
#include "platform/PlatformDefine.h"
namespace ax
{
// 新增常量定义
namespace gameex
{
constexpr uint8_t BASE_OFFSET      = 0x3C;  // 编码偏移量
constexpr uint8_t MAX_ENCODE_VALUE = 0x3F;  // 6位最大值
AX_DLL std::string Encode6BitStr(std::string_view asmsg);
AX_DLL std::string Decode6BitStr(std::string_view asmsg);
}  // namespace gameex
}  // namespace ax
#endif