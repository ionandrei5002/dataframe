#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <iostream>
#include <set>

#include "value.h"

class Aggregator
{
public:
    virtual ~Aggregator(){}
    virtual void input(const Value* input) = 0;
    virtual Value* output() = 0;
    virtual void reset() = 0;
};

template<typename T, typename U = T>
class None: public Aggregator
{
private:
    TypedValue<U> val;
public:
    void input(const Value* input) override
    {
        bytebuffer buff = const_cast<Value*>(input)->getValue();
        val.setValue(buff);
    }
    Value* output() override
    {
        return reinterpret_cast<Value*>(&val);
    }
    void reset() override
    {
    }
};

template<typename T, typename U = UInt64Type>
class DistinctCounter: public Aggregator
{
private:
    TypedValue<U> val;
    std::set<TypedValue<T>> set;
public:
    void input(const Value* input) override
    {
        TypedValue<T> inputValue = *reinterpret_cast<TypedValue<T>*>(input);
        set.insert(inputValue);
    }
    Value* output() override
    {
        uint64_t setSize = set.size();
        uint64_t typeSize = sizeof(setSize);
        val.setValue(reinterpret_cast<const char*>(&setSize), typeSize);

        return reinterpret_cast<Value*>(&val);
    }
    void reset()
    {
        set.clear();
    }
};

template<typename T, typename U = UInt64Type>
class Sum: public Aggregator
{
private:
    TypedValue<U> val;
    uint64_t sum = 0;
public:
    void input(const Value* input) override
    {
        TypedValue<T> inputValue = *reinterpret_cast<const TypedValue<T>*>(input);
        sum += *reinterpret_cast<uint64_t*>(inputValue.getValue()._buffer);
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
};

#endif // AGGREGATOR_H
