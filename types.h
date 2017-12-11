#ifndef TYPES
#define TYPES

#include <memory>
#include "bytebuffer.h"

struct Type
{
    enum type
    {
        UINT8,
        INT8,
        UINT32,
        INT32,
        UINT64,
        INT64,
        STRING
    };
};

template<int TYPE>
struct type_traits
{
};


template<>
struct type_traits<Type::UINT8>
{
    typedef uint8_t value_type;
    static constexpr char* name = const_cast<char*>("UINT8");
    static constexpr int value_byte_size = 1;
};

template<>
struct type_traits<Type::INT8>
{
    typedef int8_t value_type;
    static constexpr char* name = const_cast<char*>("INT8");
    static constexpr int value_byte_size = 1;
};

template<>
struct type_traits<Type::UINT32>
{
    typedef uint32_t value_type;
    static constexpr char* name = const_cast<char*>("UINT32");
    static constexpr int value_byte_size = 4;
};

template<>
struct type_traits<Type::INT32>
{
    typedef int32_t value_type;
    static constexpr char* name = const_cast<char*>("INT32");
    static constexpr int value_byte_size = 4;
};

template<>
struct type_traits<Type::UINT64>
{
    typedef uint64_t value_type;
    static constexpr char* name = const_cast<char*>("UINT64");
    static constexpr int value_byte_size = 8;
};

template<>
struct type_traits<Type::INT64>
{
    typedef int64_t value_type;
    static constexpr char* name = const_cast<char*>("INT64");
    static constexpr int value_byte_size = 8;
};

template<>
struct type_traits<Type::STRING>
{
    typedef bytebuffer value_type;
    static constexpr char* name = const_cast<char*>("STRING");
    static constexpr int value_byte_size = sizeof(bytebuffer);
};

template<Type::type TYPE>
struct DataType
{
    static constexpr Type::type type_num = TYPE;
    static constexpr char* name = type_traits<TYPE>::name;
    typedef typename type_traits<TYPE>::value_type c_type;
};

typedef DataType<Type::UINT8> UInt8Type;
typedef DataType<Type::INT8> Int8Type;
typedef DataType<Type::UINT32> UInt32Type;
typedef DataType<Type::INT32> Int32Type;
typedef DataType<Type::UINT64> UInt64Type;
typedef DataType<Type::INT64> Int64Type;
typedef DataType<Type::STRING> StringType;

#endif // TYPES
