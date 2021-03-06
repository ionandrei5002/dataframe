#ifndef COLUMN_H
#define COLUMN_H

#include <vector>
#include <set>

#include "types.h"
#include "value.h"

class Column
{
public:
    std::vector<char> _column;
    std::vector<uint64_t> _position;
    uint64_t nb_elements = 0;
public:
    Column():_column(std::vector<char>()),_position(std::vector<uint64_t>()),nb_elements(0){}
    virtual ~Column(){}
    static std::unique_ptr<Column> factory(Type::type type);
    virtual void putValue(Value* value) = 0;
    virtual void putValue(const char* value, uint64_t size) = 0;
    virtual Value* getValue(size_t pos) = 0;
    virtual Type::type getType() = 0;
};

template<typename T>
class TypedColumn: public Column
{
private:
    TypedValue<T> _typedvalue;
    Type::type _type = T::type_num;
public:
    TypedColumn():_typedvalue(TypedValue<T>()){
        Column::Column();
    }
    TypedColumn(const std::unique_ptr<Column>& val):_typedvalue(TypedValue<T>())
    {
        _column = val->_column;
        _position = val->_position;
        nb_elements = val->nb_elements;
    }
    Type::type getType() override { return _type; }
    void putValue(Value* value) override;
    void putValue(const char* value, uint64_t size) override;
    Value* getValue(size_t pos) override;
};

template class TypedColumn<UInt8Type>;
template class TypedColumn<Int8Type>;
template class TypedColumn<UInt16Type>;
template class TypedColumn<Int16Type>;
template class TypedColumn<UInt32Type>;
template class TypedColumn<Int32Type>;
template class TypedColumn<UInt64Type>;
template class TypedColumn<Int64Type>;
template class TypedColumn<FloatType>;
template class TypedColumn<DoubleType>;
template class TypedColumn<StringType>;

#endif // COLUMN_H
