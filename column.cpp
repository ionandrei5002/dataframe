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
    bytebuffer buffer = _value.getValue();
    for(uint64_t i = 0; i < buffer._size; i++)
        _column.push_back(buffer._buffer[i]);

    nb_elements++;
}

template<typename T>
void TypedColumn<T>::putValue(const char* value, uint64_t size)
{
    TypedValue<T> _value;
    _value.setValue(value, size);
    bytebuffer buffer = _value.getValue();
    for(uint64_t i = 0; i < buffer._size; i++)
        _column.push_back(buffer._buffer[i]);

    nb_elements++;
}

template<typename T>
std::unique_ptr<Value> TypedColumn<T>::getValue(size_t pos)
{
    typedef typename T::c_type type;
    uint64_t _size = sizeof(type);

    const char* _value_position = reinterpret_cast<char*>(&_column[pos * _size]);
    TypedValue<T> value;
    value.setValue(_value_position, _size);

    return std::make_unique<TypedValue<T>>(value);
}

template<>
void TypedColumn<StringType>::putValue(std::unique_ptr<Value> value)
{
    TypedValue<StringType> _value = *reinterpret_cast<TypedValue<StringType>*>(value.get());
    bytebuffer buffer = _value.getValue();
    uint8_t* size = reinterpret_cast<uint8_t*>(&buffer._size);

    uint64_t last_index = _column.size();

    for(uint64_t i = 0; i < sizeof(buffer._size); i++)
        _column.push_back(size[i]);
    for(uint64_t i = 0; i < buffer._size; i++)
        _column.push_back(buffer._buffer[i]);

    _position.push_back(last_index);

    nb_elements++;
}

template<>
void TypedColumn<StringType>::putValue(const char* value, uint64_t size)
{
    TypedValue<StringType> _value;
    _value.setValue(value, size);
    bytebuffer buffer = _value.getValue();

    uint64_t last_index = _column.size();

    uint8_t* _size = reinterpret_cast<uint8_t*>(&buffer._size);
    for(uint64_t i = 0; i < sizeof(buffer._size); i++)
        _column.push_back(_size[i]);
    for(uint64_t i = 0; i < buffer._size; i++)
        _column.push_back(buffer._buffer[i]);

    _position.push_back(last_index);

    nb_elements++;
}

template<>
std::unique_ptr<Value> TypedColumn<StringType>::getValue(size_t pos)
{
    uint64_t buffer_position = _position[pos];
    uint64_t _size = *reinterpret_cast<uint64_t*>(&_column[buffer_position]);
    const char* _value_position = reinterpret_cast<char*>(&_column[buffer_position + sizeof(_size)]);
    TypedValue<StringType> value;
    value.setValue(_value_position, _size);
    return std::make_unique<TypedValue<StringType>>(value);
}
