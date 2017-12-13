#ifndef VALUE_H
#define VALUE_H

#include <memory>

#include "types.h"
#include "bytebuffer.h"

class Value
{
public:
    virtual ~Value(){}
    virtual void setValue(const char* value, uint64_t size) = 0;
    virtual bytebuffer getValue() = 0;
};

template<typename T>
class TypedValue: public Value {
private:
    typename T::c_type _value;
public:
    void setValue(const char* value, uint64_t size) override;
    bytebuffer getValue() override;
};

template<>
class TypedValue<StringType>: public Value {
private:
    StringType::c_type _value;
public:
    void setValue(const char* value, uint64_t size) override;
    bytebuffer getValue() override;
};

template class TypedValue<UInt8Type>;
template class TypedValue<Int8Type>;
template class TypedValue<UInt32Type>;
template class TypedValue<Int32Type>;
template class TypedValue<UInt64Type>;
template class TypedValue<Int64Type>;
template class TypedValue<StringType>;

#endif // VALUE_H
