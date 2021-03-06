#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <experimental/string_view>

#include "types.h"
#include "bytebuffer.h"

class Value
{
public:
    virtual ~Value(){}
    virtual void setValue(const char* value, uint64_t size) = 0;
    virtual void setValue(bytebuffer buff) = 0;
    virtual bytebuffer getValue() = 0;
    friend std::ostream& operator <<(std::ostream& output, const Value& val);
    virtual void print(std::ostream& output) const = 0;
    virtual Type::type getType() = 0;
};

template<typename T>
class TypedValue: public Value {
private:
    typename T::c_type _value;
    typedef typename T::c_type type;
public:
    TypedValue():_value(type(0)){}
    void setValue(const char* value, uint64_t size) override;
    void setValue(bytebuffer buff) override;
    bytebuffer getValue() override;
    void print(std::ostream& output) const override;
    bool operator <(const TypedValue<T>& val)
    {
        return _value < val._value;
    }
    friend bool operator <(const TypedValue<T>& lv, const TypedValue<T>& rv)
    {
        return lv._value < rv._value;
    }
    bool operator ==(const TypedValue<T>& val)
    {
        return !(_value < val._value) && !(val._value < _value);
    }
    bool operator !=(const TypedValue<T>& val)
    {
        return !this->operator ==(val);
    }
    Type::type getType() override
    {
        return T::type_num;
    }
};

template<>
class TypedValue<StringType>: public Value {
private:
    StringType::c_type _value;
    typedef typename StringType::c_type type;
public:
    TypedValue():_value(type()){}
    void setValue(const char* value, uint64_t size) override;
    void setValue(bytebuffer buff) override;
    bytebuffer getValue() override;
    void print(std::ostream& output) const override;
    bool operator <(const TypedValue<StringType>& val)
    {
        return std::experimental::string_view(reinterpret_cast<char*>(this->_value._buffer), this->_value._size)
                < std::experimental::string_view(reinterpret_cast<char*>(val._value._buffer), val._value._size);
    }
    friend bool operator <(const TypedValue<StringType>& lv, const TypedValue<StringType>& rv)
    {
        return std::experimental::string_view(reinterpret_cast<char*>(lv._value._buffer), lv._value._size)
                < std::experimental::string_view(reinterpret_cast<char*>(rv._value._buffer), rv._value._size);
    }
    bool operator ==(const TypedValue<StringType>& val)
    {
        return std::experimental::string_view(reinterpret_cast<char*>(this->_value._buffer), this->_value._size)
                == std::experimental::string_view(reinterpret_cast<char*>(val._value._buffer), val._value._size);
    }
    bool operator !=(const TypedValue<StringType>& val)
    {
        return !this->operator ==(val);
    }
    Type::type getType() override
    {
        return StringType::type_num;
    }
};

template class TypedValue<UInt8Type>;
template class TypedValue<Int8Type>;
template class TypedValue<UInt16Type>;
template class TypedValue<Int16Type>;
template class TypedValue<UInt32Type>;
template class TypedValue<Int32Type>;
template class TypedValue<UInt64Type>;
template class TypedValue<Int64Type>;
template class TypedValue<FloatType>;
template class TypedValue<DoubleType>;
template class TypedValue<StringType>;

#endif // VALUE_H
