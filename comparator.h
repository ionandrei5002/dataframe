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
    virtual bool operator ()(const uint32_t first, const uint32_t second) = 0;
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
    bool operator ()(const uint32_t first, const uint32_t second) override;
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
