#include "comparator.h"

template<typename T>
bool TypedComparator<T>::operator()(const uint32_t first, const uint32_t second)
{
    TypedValue<T> lv = *reinterpret_cast<TypedValue<T>*>(_column->getValue(first));
    TypedValue<T> rv = *reinterpret_cast<TypedValue<T>*>(_column->getValue(second));
    return lv < rv;
}

template<typename T>
bool TypedComparator<T>::equal(const uint32_t first, const uint32_t second)
{
    TypedValue<T> lv = *reinterpret_cast<TypedValue<T>*>(_column->getValue(first));
    TypedValue<T> rv = *reinterpret_cast<TypedValue<T>*>(_column->getValue(second));
    return (lv == rv);
}

template<typename T>
bool TypedComparator<T>::differit(const uint32_t first, const uint32_t second)
{
    TypedValue<T> lv = *reinterpret_cast<TypedValue<T>*>(_column->getValue(first));
    TypedValue<T> rv = *reinterpret_cast<TypedValue<T>*>(_column->getValue(second));
    return (lv != rv);
}
