#ifndef COMPARATOR_H
#define COMPARATOR_H

#include <vector>

#include "types.h"
#include "value.h"
#include "column.h"

class Comparator
{
public:
    Comparator(): _sorter(sort(this)){}
    virtual ~Comparator(){}
    virtual Type::type getType() = 0;
    virtual bool operator ()(const uint32_t first, const uint32_t second) = 0;
    virtual bool equal(const uint32_t first, const uint32_t second) = 0;
    virtual bool differit(const uint32_t first, const uint32_t second) = 0;
    struct sort
    {
        Comparator* _comp;
        sort(Comparator* comp): _comp(comp){}
        bool operator ()(const uint32_t first, const uint32_t second)
        {
            return _comp->operator ()(first, second);
        }
    } _sorter;
};

template<typename T>
class TypedComparator: public Comparator
{
private:
    Type::type _type = T::type_num;
    std::shared_ptr<Column> _column;
public:
    TypedComparator(std::shared_ptr<Column> column): _column(column)
    {
        _sorter = sort(this);
    }
    TypedComparator(const TypedComparator<T>& val)
    {
        _sorter = sort(this);
        _column = val._column;
    }
    Type::type getType() override { return _type; }
    bool operator ()(const uint32_t first, const uint32_t second) override;
    bool equal(const uint32_t first, const uint32_t second) override;
    bool differit(const uint32_t first, const uint32_t second) override;
};

class Sorter
{
private:
    std::vector<std::shared_ptr<Comparator>> _comparators;
public:
    Sorter(std::vector<std::shared_ptr<Comparator>>& comparators): _comparators(comparators){}
    inline bool operator ()(const uint32_t first, const uint32_t second) {
        bool _comp = false;

        for(auto it = _comparators.begin(); it != _comparators.end(); ++it)
        {
            if ((*it)->differit(first, second))
            {
                _comp = (*it)->_sorter.operator ()(first, second);
                return _comp;
            }
        }

        return _comp;
    }
};

class ValueComparator {
protected:
    Value* this_value;
public:
    ValueComparator(Value* value):this_value(value){}
    virtual ~ValueComparator(){this_value = nullptr;}
    virtual Type::type getType() = 0;
    void setValue(Value* value)
    {
        this_value = value;
    }
    virtual bool operator <(const Value* value) = 0;
    virtual bool operator ==(const Value* value) = 0;
    virtual bool operator !=(const Value* value) = 0;
};

template<typename T>
class TypedValueComparator: public ValueComparator {
private:
    Type::type _type = T::type_num;
public:
    TypedValueComparator():ValueComparator(nullptr){}
    TypedValueComparator(Value* value):ValueComparator(value){}
    Type::type getType() override { return _type; }
    bool operator <(const Value* value) override
    {
        return *reinterpret_cast<TypedValue<T>*>(this_value) < *reinterpret_cast<const TypedValue<T>*>(value);
    }
    bool operator ==(const Value* value) override
    {
        return *reinterpret_cast<TypedValue<T>*>(this_value) == *reinterpret_cast<const TypedValue<T>*>(value);
    }
    bool operator !=(const Value* value) override
    {
        return (*reinterpret_cast<TypedValue<T>*>(this_value)) != (*reinterpret_cast<const TypedValue<T>*>(value));
    }
};

template class TypedComparator<UInt8Type>;
template class TypedComparator<Int8Type>;
template class TypedComparator<UInt16Type>;
template class TypedComparator<Int16Type>;
template class TypedComparator<UInt32Type>;
template class TypedComparator<Int32Type>;
template class TypedComparator<UInt64Type>;
template class TypedComparator<Int64Type>;
template class TypedComparator<FloatType>;
template class TypedComparator<DoubleType>;
template class TypedComparator<StringType>;

#endif // COMPARATOR_H
