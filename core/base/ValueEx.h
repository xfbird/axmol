//---------------------------------------------------------------------------
// ValueEx.h
#pragma once
#include "platform/PlatformMacros.h"
#include "base/Macros.h"
#include <string>
#include <cstdint> // For fixed-width integer types
#include <stdexcept> // 引入标准异常头文件，用于异常处理

NS_AX_BEGIN

class AX_DLL ValueEx
{
public:
    enum Type
    {
        NONE = 0,
        CHAR,
        UCHAR,
        BOOLEAN,
        INT,
        UINT,
        INT64,
        UINT64,
        FLOAT,
        DOUBLE,
        STRING
    };

    ValueEx();
    explicit ValueEx(bool value);
    explicit ValueEx(int8_t value);
    explicit ValueEx(uint8_t value);
    explicit ValueEx(int16_t value);
    explicit ValueEx(uint16_t value);
    explicit ValueEx(int32_t value);
    explicit ValueEx(uint32_t value);
    explicit ValueEx(int64_t value);
    explicit ValueEx(uint64_t value);
    explicit ValueEx(float value);
    explicit ValueEx(double value);
    explicit ValueEx(std::string_view value);
    explicit ValueEx(std::string value);
    explicit ValueEx(std::string_view value,u_char ntype);
    explicit ValueEx(std::string value,u_char ntype);
    ValueEx(const ValueEx& other);
    ValueEx& operator=(const ValueEx& other);
    ValueEx(ValueEx&& other) noexcept;
    ValueEx& operator=(ValueEx&& other) noexcept;
    ~ValueEx();

    bool asBool() const;
    int8_t asChar() const;
    uint8_t asUChar() const;
    int16_t asInt16() const;
    uint16_t asUInt16() const;
    int32_t asInt32() const;
    uint32_t asUInt32() const;
    int asInt() const { return asInt32();}
    unsigned int asUInt() const { return asUInt32();}    

    int64_t asInt64() const;
    uint64_t asUInt64() const;
    float asFloat() const;
    double asDouble() const;
    std::string asString() const;

    Type getType() const;
    void setType(Type type);
    void setType(u_char ntype);

private:
    void convertFromString(std::string value, Type type);
    Type _vtype;
    union Data {
        bool bVal;
        int8_t cVal;
        uint8_t ucVal;
        int16_t isVal;
        uint16_t usVal;
        int32_t iVal;
        uint32_t uVal;
        int64_t i64Val;
        uint64_t u64Val;
        float fVal;
        double dVal;
    } data;
    std::string strVal;
    
    // 新增：用于安全访问数据的私有函数
    template<typename T>
    T& getAs() noexcept;
    template<typename T>
    const T& getAs() const noexcept;
};

NS_AX_END
