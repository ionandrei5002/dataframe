#include "value.h"

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
