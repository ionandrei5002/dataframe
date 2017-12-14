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
    case Type::UINT16:
        value = std::make_unique<TypedColumn<UInt16Type>>();
        break;
    case Type::INT16:
        value = std::make_unique<TypedColumn<Int16Type>>();
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
    case Type::FLOAT:
        value = std::make_unique<TypedColumn<FloatType>>();
        break;
    case Type::DOUBLE:
        value = std::make_unique<TypedColumn<DoubleType>>();
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
    typedef typename T::c_type type;
    std::string val(value, size);

    const type num_val = static_cast<type>(std::stoi(val));

    TypedValue<T> _value;
    _value.setValue(reinterpret_cast<const char*>(&num_val), sizeof(type));
    bytebuffer buffer = _value.getValue();
    for(uint64_t i = 0; i < buffer._size; i++)
        _column.push_back(buffer._buffer[i]);

    nb_elements++;
}

template<>
void TypedColumn<UInt64Type>::putValue(const char* value, uint64_t size)
{
    typedef typename UInt64Type::c_type type;
    std::string val(value, size);

    const type num_val = static_cast<type>(std::stoul(val));

    TypedValue<UInt64Type> _value;
    _value.setValue(reinterpret_cast<const char*>(&num_val), sizeof(type));
    bytebuffer buffer = _value.getValue();
    for(uint64_t i = 0; i < buffer._size; i++)
        _column.push_back(buffer._buffer[i]);

    nb_elements++;
}

template<>
void TypedColumn<Int64Type>::putValue(const char* value, uint64_t size)
{
    typedef typename Int64Type::c_type type;
    std::string val(value, size);

    const type num_val = static_cast<type>(std::stol(val));

    TypedValue<Int64Type> _value;
    _value.setValue(reinterpret_cast<const char*>(&num_val), sizeof(type));
    bytebuffer buffer = _value.getValue();
    for(uint64_t i = 0; i < buffer._size; i++)
        _column.push_back(buffer._buffer[i]);

    nb_elements++;
}

template<>
void TypedColumn<FloatType>::putValue(const char* value, uint64_t size)
{
    std::string val(value, size);
    const float num_val = std::stof(val);

    TypedValue<FloatType> _value;
    _value.setValue(reinterpret_cast<const char*>(&num_val), sizeof(float));
    bytebuffer buffer = _value.getValue();
    for(uint64_t i = 0; i < buffer._size; i++)
        _column.push_back(buffer._buffer[i]);

    nb_elements++;
}

template<>
void TypedColumn<DoubleType>::putValue(const char* value, uint64_t size)
{
    std::string val(value, size);
    const double num_val = std::stod(val);

    TypedValue<DoubleType> _value;
    _value.setValue(reinterpret_cast<const char*>(&num_val), sizeof(double));
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
std::unique_ptr<Value> TypedColumn<FloatType>::getValue(size_t pos)
{
    typedef typename FloatType::c_type type;
    uint64_t _size = sizeof(type);

    const char* _value_position = reinterpret_cast<char*>(&_column[pos * _size]);

    TypedValue<FloatType> value;

    value.setValue(_value_position, _size);

    return std::make_unique<TypedValue<FloatType>>(value);
}

template<>
std::unique_ptr<Value> TypedColumn<DoubleType>::getValue(size_t pos)
{
    typedef typename DoubleType::c_type type;
    uint64_t _size = sizeof(type);

    const char* _value_position = reinterpret_cast<char*>(&_column[pos * _size]);

    TypedValue<DoubleType> value;

    value.setValue(_value_position, _size);

    return std::make_unique<TypedValue<DoubleType>>(value);
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
