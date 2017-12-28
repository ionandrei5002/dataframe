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
    virtual void input(const Value* input) = 0;
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
    void input(const Value* input) override
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
    void input(const Value* input) override
    {
        const TypedValue<T>* inputValue = static_cast<const TypedValue<T>*>(input);
        set.insert(*inputValue);
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
    void input(const Value* input) override
    {
        TypedValue<StringType>* inputValue = static_cast<TypedValue<StringType>*>(const_cast<Value*>(input));
        set.insert(std::string(inputValue->getValue()._buffer, inputValue->getValue()._size));
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
    TypedValue<U> val = TypedValue<U>();
    uint64_t sum = 0;
    Type::type _input_type = T::type_num;
    Type::type _output_type = U::type_num;
public:
    void input(const Value* input) override
    {
        TypedValue<T>* inputValue = reinterpret_cast<TypedValue<T>*>(const_cast<Value*>(input));
        sum += *reinterpret_cast<uint64_t*>(inputValue->getValue()._buffer);
    }
    Value* output() override
    {
        uint64_t typeSize = sizeof(sum);
        val.setValue(reinterpret_cast<const char*>(&sum), typeSize);

        return reinterpret_cast<Value*>(&val);
    }
    void reset()
    {
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

template std::unique_ptr<Aggregator> Aggregator::factory<UInt8Type>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<Int8Type>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<UInt16Type>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<Int16Type>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<UInt32Type>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<Int32Type>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<UInt64Type>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<Int64Type>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<FloatType>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<DoubleType>(AggType::type);
template std::unique_ptr<Aggregator> Aggregator::factory<StringType>(AggType::type);

#endif // AGGREGATOR_H
