#include "column.h"

std::unique_ptr<Column> Column::factory(Type::type type)
{
    std::unique_ptr<Column> value;
    switch (type) {
    case Type::UINT8:
        value = std::make_unique<TypedColumn<UInt8Type>>();
        break;
    case Type::INT8:
        value = std::make_unique<TypedColumn<Int8Type>>();
        break;
    case Type::UINT32:
        value = std::make_unique<TypedColumn<UInt32Type>>();
        break;
    case Type::INT32:
        value = std::make_unique<TypedColumn<Int32Type>>();
        break;
    case Type::UINT64:
        value = std::make_unique<TypedColumn<UInt64Type>>();
        break;
    case Type::INT64:
        value = std::make_unique<TypedColumn<UInt64Type>>();
        break;
    case Type::STRING:
        value = std::make_unique<TypedColumn<StringType>>();
        break;
    default:
        value = nullptr;
        break;
    }

    return value;
}

template<typename T>
void TypedColumn<T>::putValue(std::unique_ptr<Value> value)
{
    TypedValue<T> _value = *reinterpret_cast<TypedValue<T>*>(value.get());
    _column.push_back(_value);
}

template<typename T>
void TypedColumn<T>::putValue(const char* value, uint64_t size)
{
    TypedValue<T> _value;
    _value.setValue(value, size);
    _column.push_back(_value);
}

template<typename T>
std::unique_ptr<Value> TypedColumn<T>::getValue(size_t pos)
{
    return std::make_unique<TypedValue<T>>(_column.at(pos));
}

template<>
void TypedColumn<StringType>::putValue(std::unique_ptr<Value> value)
{
    TypedValue<StringType> _value = *reinterpret_cast<TypedValue<StringType>*>(value.get());
    _column.push_back(_value);
}

template<>
void TypedColumn<StringType>::putValue(const char* value, uint64_t size)
{
    TypedValue<StringType> _value;
    _value.setValue(value, size);
    _column.push_back(_value);
}

template<>
std::unique_ptr<Value> TypedColumn<StringType>::getValue(size_t pos)
{
    return std::make_unique<TypedValue<StringType>>(_column.at(pos));
}
