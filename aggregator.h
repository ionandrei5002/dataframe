#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <iostream>
#include <set>

#include "value.h"

struct AggType
{
    enum type
    {
        NONE = 00,
        SUM = 10,
        DISTINCTC_COUNT = 20
    };
};

class Aggregator
{
public:
    virtual ~Aggregator(){}
    virtual void input(Value* input) = 0;
    virtual Value* output() = 0;
    virtual void reset() = 0;
    virtual Type::type inputType() = 0;
    virtual Type::type outputType() = 0;

    template<typename T>
    static std::unique_ptr<Aggregator> factory(AggType::type type);
};

template<typename T, typename U = T>
class None: public Aggregator
{
private:
    TypedValue<U> val = TypedValue<U>();
    Type::type _input_type = T::type_num;
    Type::type _output_type = U::type_num;
public:
    void input(Value* input) override
    {
        bytebuffer buff = const_cast<Value*>(input)->getValue();
        val.setValue(buff);
    }
    Value* output() override
    {
        return static_cast<Value*>(&val);
    }
    void reset() override
    {
    }
    Type::type inputType() override { return _input_type; }
    Type::type outputType() override { return _output_type; }
};

template<typename T, typename U = UInt64Type>
class DistinctCounter: public Aggregator
{
private:
    TypedValue<U> val = TypedValue<U>();
    std::set<TypedValue<T>> set;
    uint64_t setSize = 0;
    Type::type _input_type = T::type_num;
    Type::type _output_type = U::type_num;
public:
    void input(Value* input) override
    {
        const TypedValue<T> inputValue = *static_cast<const TypedValue<T>*>(input);
        set.insert(inputValue);
    }
    Value* output() override
    {
        setSize = set.size();
        uint64_t typeSize = sizeof(setSize);
        val.setValue(reinterpret_cast<const char*>(&setSize), typeSize);

        return static_cast<Value*>(&val);
    }
    void reset()
    {
        val = TypedValue<U>();
        setSize = 0;
        set.clear();
    }
    Type::type inputType() override { return _input_type; }
    Type::type outputType() override { return _output_type; }
};

template<>
class DistinctCounter<StringType, UInt64Type>: public Aggregator
{
private:
    TypedValue<UInt64Type> val = TypedValue<UInt64Type>();
    std::set<std::string> set;
    uint64_t setSize = 0;
    Type::type _input_type = StringType::type_num;
    Type::type _output_type = UInt64Type::type_num;
public:
    void input(Value* input) override
    {
        TypedValue<StringType> inputValue = *static_cast<TypedValue<StringType>*>(const_cast<Value*>(input));
        set.insert(std::string(inputValue.getValue()._buffer, inputValue.getValue()._size));
    }
    Value* output() override
    {
        setSize = set.size();
        uint64_t typeSize = sizeof(uint64_t);
        val.setValue(reinterpret_cast<const char*>(&setSize), typeSize);

        return static_cast<Value*>(&val);
    }
    void reset()
    {
        val = TypedValue<UInt64Type>();
        setSize = 0;
        set.clear();
    }
    Type::type inputType() override { return _input_type; }
    Type::type outputType() override { return _output_type; }
};

template<typename T, typename U = UInt64Type>
class Sum: public Aggregator
{
private:
    typedef typename T::c_type type;
    TypedValue<U> val = TypedValue<U>();
    TypedValue<T> inputValue = TypedValue<T>();
    uint64_t sum = 0;
    Type::type _input_type;
    Type::type _output_type;
public:
    Sum():val(TypedValue<U>()),inputValue(TypedValue<T>()),sum(0),_input_type(T::type_num),_output_type(U::type_num){}
    void input(Value* input) override
    {
        inputValue.setValue(input->getValue());

        sum += static_cast<uint64_t>(*reinterpret_cast<type*>(inputValue.getValue()._buffer));
    }
    Value* output() override
    {
        uint64_t typeSize = sizeof(sum);
        val.setValue(reinterpret_cast<const char*>(&sum), typeSize);

        return static_cast<Value*>(&val);
    }
    void reset()
    {
        val = TypedValue<U>();
        inputValue = TypedValue<T>();
        sum = 0;
    }
    Type::type inputType() override { return _input_type; }
    Type::type outputType() override { return _output_type; }
};

template<>
class Sum<StringType, UInt64Type>: public Aggregator
{
private:
    typename StringType::c_type type;
    TypedValue<UInt64Type> val = TypedValue<UInt64Type>();
    TypedValue<StringType> inputValue = TypedValue<StringType>();
    uint64_t sum = 0;
    Type::type _input_type;
    Type::type _output_type;
public:
    Sum():val(TypedValue<UInt64Type>()),inputValue(TypedValue<StringType>()),sum(0),_input_type(StringType::type_num),_output_type(UInt64Type::type_num){}
    void input(Value* input) override
    {
    }
    Value* output() override
    {
        return static_cast<Value*>(&val);
    }
    void reset()
    {
        val = TypedValue<UInt64Type>();
        inputValue = TypedValue<StringType>();
        sum = 0;
    }
    Type::type inputType() override { return _input_type; }
    Type::type outputType() override { return _output_type; }
};

template<typename T>
std::unique_ptr<Aggregator> Aggregator::factory(AggType::type type)
{
    std::unique_ptr<Aggregator> value;

    switch (type) {
    case AggType::NONE:
        value = std::make_unique<None<T>>(None<T>());
        break;
    case AggType::SUM:
        value = std::make_unique<Sum<T>>(Sum<T>());
        break;
    case AggType::DISTINCTC_COUNT:
        value = std::make_unique<DistinctCounter<T>>(DistinctCounter<T>());
        break;
    default:
        value = nullptr;
        break;
    }

    return value;
}

#endif // AGGREGATOR_H
