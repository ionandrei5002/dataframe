#include "value.h"

std::ostream& operator <<(std::ostream& output, const Value& val)
{
    val.print(output);
    return output;
}

template<typename T>
void TypedValue<T>::print(std::ostream& output) const
{
    output << _value;
}

void TypedValue<StringType>::print(std::ostream& output) const
{
    output << _value;
}

template<typename T>
bytebuffer TypedValue<T>::getValue()
{
    uint64_t size = sizeof(_value);
    char* _val = reinterpret_cast<char*>(&_value);

    bytebuffer buffer(size, _val);

    return buffer;
}

bytebuffer TypedValue<StringType>::getValue()
{
    uint64_t size = _value._size;
    char* _val = _value._buffer;

    bytebuffer buffer(size, _val);

    return buffer;
}

template<typename T>
void TypedValue<T>::setValue(const char *value, uint64_t size)
{
    this->_value = *reinterpret_cast<const type*>(value);
}

template<>
void TypedValue<UInt64Type>::setValue(const char *value, uint64_t size)
{
    this->_value = *reinterpret_cast<const type*>(value);
}

template<typename T>
void TypedValue<T>::setValue(bytebuffer buff)
{
    this->_value = *reinterpret_cast<const type*>(buff._buffer);
}

void TypedValue<StringType>::setValue(const char *value, uint64_t size)
{
    _value = bytebuffer(size, const_cast<char*>(value));
}

void TypedValue<StringType>::setValue(bytebuffer buff)
{
    _value = buff;
}
