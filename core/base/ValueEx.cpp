//---------------------------------------------------------------------------
// ValueEx.cpp
#include "base/ValueEx.h"
#include <sstream>
#include <stdexcept> // 引入标准异常头文件

NS_AX_BEGIN

ValueEx::ValueEx() : _vtype(NONE) {}

ValueEx::ValueEx(bool value) : _vtype(BOOLEAN), data({.bVal = value}) {}

ValueEx::ValueEx(int8_t value) : _vtype(CHAR), data({.cVal = value}) {}

ValueEx::ValueEx(uint8_t value) : _vtype(UCHAR), data({.ucVal = value}) {}

ValueEx::ValueEx(int16_t value) : _vtype(INT), data({.isVal = value}) {}

ValueEx::ValueEx(uint16_t value) : _vtype(UINT), data({.usVal = value}) {}

ValueEx::ValueEx(int32_t value) : _vtype(INT), data({.iVal = value}) {}

ValueEx::ValueEx(uint32_t value) : _vtype(UINT), data({.uVal = value}) {}

ValueEx::ValueEx(int64_t value) : _vtype(INT64), data({.i64Val = value}) {}

ValueEx::ValueEx(uint64_t value) : _vtype(UINT64), data({.u64Val = value}) {}

ValueEx::ValueEx(float value) : _vtype(FLOAT), data({.fVal = value}) {}

ValueEx::ValueEx(double value) : _vtype(DOUBLE), data({.dVal = value}) {}

ValueEx::ValueEx(std::string_view value) : _vtype(STRING), strVal(value) {}

ValueEx::ValueEx(std::string value) : _vtype(STRING), strVal(std::move(value)) {}

// 私有成员函数，用于从字符串转换为指定类型
void ValueEx::convertFromString(std::string value, Type type) {
    switch (type) {
        case BOOLEAN:
            if (value == "true") {
                data.bVal = true;
            } else if (value == "false") {
                data.bVal = false;
            } else {
                throw std::invalid_argument("Invalid boolean string");
            }
            break;
        case CHAR:
            if (value.size() != 1) {
                throw std::invalid_argument("Invalid char string length");
            }
            data.cVal = value[0];
            break;
        case UCHAR:
            if (value.size() != 1) {
                throw std::invalid_argument("Invalid uchar string length");
            }
            data.ucVal = static_cast<uint8_t>(value[0]);
            break;
        case INT:
            data.iVal = std::stoi(value);
            break;
        case UINT:
            data.uVal = std::stoul(value);
            break;
        case INT64:
            data.i64Val = std::stoll(value);
            break;
        case UINT64:
            data.u64Val = std::stoull(value);
            break;
        case FLOAT:
            data.fVal = std::stof(value);
            break;
        case DOUBLE:
            data.dVal = std::stod(value);
            break;
        case STRING:
            strVal = std::string(value);
            break;
        default:
            throw std::invalid_argument("Invalid type value encountered.");
    }
}

ValueEx::ValueEx(std::string_view value, u_char ntype) :
    _vtype(static_cast<Type>(ntype))
{
    try {
        convertFromString(std::string(value), _vtype);
    } catch (const std::invalid_argument& e) {
        // AXLOGW(e.what());
        _vtype = Type::STRING;
        strVal = std::string(value);
    }
}

ValueEx::ValueEx(std::string value, u_char ntype) :
    _vtype(static_cast<Type>(ntype))
{
    try {
        convertFromString(value, _vtype);
    } catch (const std::invalid_argument& e) {
        // AXLOGW(e.what());
        _vtype = Type::STRING;
        strVal = std::move(value);
    }
}
ValueEx::ValueEx(const ValueEx& other) : _vtype(other._vtype), data(other.data), strVal(other.strVal) {}

ValueEx& ValueEx::operator=(const ValueEx& other)
{
    if (this != &other)
    {
        _vtype = other._vtype;
        data = other.data;
        strVal = other.strVal;
    }
    return *this;
}

ValueEx::ValueEx(ValueEx&& other) noexcept : _vtype(other._vtype), data(other.data), strVal(std::move(other.strVal)) {}

ValueEx& ValueEx::operator=(ValueEx&& other) noexcept
{
    if (this != &other)
    {
        _vtype = other._vtype;
        data = other.data;
        strVal = std::move(other.strVal);
    }
    return *this;
}

bool ValueEx::asBool() const
{
    return _vtype == BOOLEAN ? data.bVal : false;
}

int8_t ValueEx::asChar() const
{
    return _vtype == CHAR ? data.cVal : 0;
}

uint8_t ValueEx::asUChar() const
{
    return _vtype == UCHAR ? data.ucVal : 0;
}

int16_t ValueEx::asInt16() const
{
    return _vtype == INT ? data.isVal : 0;
}

uint16_t ValueEx::asUInt16() const
{
    return _vtype == UINT ? data.usVal : 0;
}

int32_t ValueEx::asInt32() const
{
    return _vtype == INT ? data.iVal : 0;
}

uint32_t ValueEx::asUInt32() const
{
    return _vtype == UINT ? data.uVal : 0;
}

int64_t ValueEx::asInt64() const
{
    return _vtype == INT64 ? data.i64Val : 0;
}

uint64_t ValueEx::asUInt64() const
{
    return _vtype == UINT64 ? data.u64Val : 0;
}

float ValueEx::asFloat() const
{
    return _vtype == FLOAT ? data.fVal : 0.0f;
}

double ValueEx::asDouble() const
{
    return _vtype == DOUBLE ? data.dVal : 0.0;
}

// std::string ValueEx::asString() const
// {


//     return _vtype == STRING ? strVal : "";
// }
std::string ValueEx::asString() const
{
    std::stringstream ss;
    
    switch (_vtype) {
        case STRING:
            return strVal;
        case BOOLEAN:
            ss << (data.bVal ? "true" : "false");
            break;
        case CHAR:
            ss << static_cast<int>(data.cVal);
            break;
        case UCHAR:
            ss << static_cast<int>(data.ucVal);
            break;
        case INT:
            ss << data.iVal;
            break;
        case UINT:
            ss << data.uVal;
            break;
        case INT64:
            ss << data.i64Val;
            break;
        case UINT64:
            ss << data.u64Val;
            break;
        case FLOAT:
            ss << data.fVal;
            break;
        case DOUBLE:
            ss << data.dVal;
            break;
        default:
            ss << "Unknown type";
            break;
    }
    return ss.str();
}
ValueEx::Type ValueEx::getType() const
{
    return _vtype;
}

void ValueEx::setType(Type type)
{
    _vtype = type;
}

ValueEx::~ValueEx() {}

template<typename T>
T& ValueEx::getAs() noexcept
{
    if constexpr (std::is_same_v<T, bool>) {
        if (_vtype != BOOLEAN) {
            throw std::bad_cast(); // 抛出异常，类型不匹配
        }
    } else if constexpr (std::is_same_v<T, int8_t>) {
        if (_vtype != CHAR) {
            throw std::bad_cast();
        }
    } else if constexpr (std::is_same_v<T, uint8_t>) {
        if (_vtype != UCHAR) {
            throw std::bad_cast();
        }
    } // ... 其他类型检查
    return data.template getAs<T>(); // 使用std::variant或std::any时，这里会有所不同
}

template<typename T>
const T& ValueEx::getAs() const noexcept
{
    if constexpr (std::is_same_v<T, bool>) {
        if (_vtype != BOOLEAN) {
            throw std::bad_cast();
        }
    } else if constexpr (std::is_same_v<T, int8_t>) {
        if (_vtype != CHAR) {
            throw std::bad_cast();
        }
    } else if constexpr (std::is_same_v<T, uint8_t>) {
        if (_vtype != UCHAR) {
            throw std::bad_cast();
        }
    } // ... 其他类型检查
    return data.template getAs<T>();
}

void ValueEx::setType(u_char ntype)
{
        // NONE = 0,
        // CHAR,
        // UCHAR,
        // BOOLEAN,
        // INT,
        // UINT,
        // INT64,
        // UINT64,
        // FLOAT,
        // DOUBLE,
        // STRING    
    try {
        if (ntype > Type::STRING) {
            throw std::invalid_argument("Invalid type value encountered.");          
        } else {
            _vtype = static_cast<Type>(ntype);
        }
    } catch (const std::invalid_argument& e) {
        // AXLOGW(e.what());
        _vtype = Type::STRING;
    }
}

NS_AX_END
