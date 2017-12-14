#include "value.h"

template<typename T>
void TypedValue<T>::print(std::ostream& output)
{
    output << _value;
}

void TypedValue<StringType>::print(std::ostream& output)
{
    output << _value;
}

template<typename T>
bytebuffer TypedValue<T>::getValue()
{
    uint64_t size = sizeof(_value);
    uint8_t* _val = reinterpret_cast<uint8_t*>(&_value);

    bytebuffer buffer(size, _val);

    return buffer;
}

bytebuffer TypedValue<StringType>::getValue()
{
    uint64_t size = _value._size;
    uint8_t* _val = _value._buffer;

    bytebuffer buffer(size, _val);

    return buffer;
}

template<typename T>
void TypedValue<T>::setValue(const char *value, uint64_t size)
{
    char* addr_value = reinterpret_cast<char*>(&this->_value);
    memcpy(addr_value, value, size);
}

void TypedValue<StringType>::setValue(const char *value, uint64_t size)
{
    _value = std::move<bytebuffer>(bytebuffer(size, reinterpret_cast<uint8_t*>(const_cast<char*>(value))));
}
