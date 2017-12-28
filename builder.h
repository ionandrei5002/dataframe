#ifndef BUILDER_H
#define BUILDER_H

#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>

#include "types.h"
#include "column.h"
#include "comparator.h"
#include "aggregator.h"

class Builder {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    std::vector<uint32_t> _sorting;
    std::vector<std::unique_ptr<Column>>& _source;
    std::vector<std::unique_ptr<Column>> _destination;
    std::vector<std::shared_ptr<Comparator>> _comparators;
    std::vector<std::unique_ptr<ValueComparator>> _groupby;
    std::vector<uint32_t> _group_columns;
    std::vector<std::unique_ptr<Aggregator>> _aggregators;
    std::vector<Value*> _helper;
    std::shared_ptr<Comparator> comparatorMaker(Type::type type, std::unique_ptr<Column>& column);
    std::unique_ptr<ValueComparator> valueComparatorMaker(Type::type type);
    std::unique_ptr<Aggregator> aggregatorMaker(Type::type type, AggType::type agg_type);
    void valueMaker(Type::type type);
public:
    Builder(std::vector<std::unique_ptr<Column>>& source):_source(source)
    {
        for(uint64_t i = 0; i < source.size(); i++)
        {
            Type::type operation_type = source[i]->getType();

            _aggregators.push_back(aggregatorMaker(operation_type, AggType::NONE));
        }
    }
    Builder& applySortColumn(uint32_t column)
    {
        Type::type operation_type = _source[column]->getType();

        _comparators.push_back(comparatorMaker(operation_type, _source[column]));

        return *this;
    }
    Builder& applyGroupByColumn(uint32_t column)
    {
        _group_columns.push_back(column);

        return *this;
    }
    Builder& applyAggregateColumn(uint32_t column, AggType::type type)
    {
        Type::type operation_type = _source[column]->getType();

        _aggregators[column] = aggregatorMaker(operation_type, type);

        return *this;
    }
    std::vector<std::unique_ptr<Column>> run()
    {
        sort();
        buildDestination();
        buildHelper();
        groupby();

        return std::move(this->_destination);
    }
private:
    void sort()
    {
        uint64_t rows = _source[0]->nb_elements;

        for(uint64_t i = 0; i < rows; i++)
        {
            _sorting.push_back(i);
        }

        start = std::chrono::high_resolution_clock::now();

        std::sort(_sorting.begin(), _sorting.end(), Sorter(_comparators));

        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = end - start;

        std::cout << "sort duration = " << elapsed_time.count() << "s" << std::endl;
    }

    void buildDestination()
    {
        for(uint64_t i = 0; i < _source.size(); i++)
        {
            _destination.push_back(Column::factory(_aggregators[i]->outputType()));
        }

        for(uint64_t i = 0; i < _source.size(); i++)
        {
            _groupby.push_back(valueComparatorMaker(_source[i]->getType()));
        }
    }

    void buildHelper()
    {
        for(uint64_t i = 0; i < _source.size(); i++)
        {
            valueMaker(_source[i]->getType());
        }
    }

    void groupby()
    {
        for(uint64_t i = 0; i < _source.size(); i++)
        {
            std::cout << i << " " << _source[i]->getType() << " " << _helper[i]->getType() << " " << _groupby[i]->getType() << " " << _aggregators[i]->outputType() << " " << _destination[i]->getType() << std::endl;
        }

        start = std::chrono::high_resolution_clock::now();
        GroupBy groupby(_source, _destination, _sorting, _groupby, _group_columns, _aggregators, _helper);
        groupby.run();
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = end - start;

        std::cout << "group duration = " << elapsed_time.count() << "s" << std::endl;
    }
};

std::shared_ptr<Comparator> Builder::comparatorMaker(Type::type type, std::unique_ptr<Column>& column)
{
    std::shared_ptr<Comparator> value;
    switch (type) {
    case Type::UINT8:
        value = std::make_shared<TypedComparator<UInt8Type>>(std::make_shared<TypedColumn<UInt8Type>>(column));
        break;
    case Type::INT8:
        value = std::make_shared<TypedComparator<Int8Type>>(std::make_shared<TypedColumn<Int8Type>>(column));
        break;
    case Type::UINT16:
        value = std::make_shared<TypedComparator<UInt16Type>>(std::make_shared<TypedColumn<UInt16Type>>(column));
        break;
    case Type::INT16:
        value = std::make_shared<TypedComparator<Int16Type>>(std::make_shared<TypedColumn<Int16Type>>(column));
        break;
    case Type::UINT32:
        value = std::make_shared<TypedComparator<UInt32Type>>(std::make_shared<TypedColumn<UInt32Type>>(column));
        break;
    case Type::INT32:
        value = std::make_shared<TypedComparator<Int32Type>>(std::make_shared<TypedColumn<Int32Type>>(column));
        break;
    case Type::UINT64:
        value = std::make_shared<TypedComparator<UInt64Type>>(std::make_shared<TypedColumn<UInt64Type>>(column));
        break;
    case Type::INT64:
        value = std::make_shared<TypedComparator<Int64Type>>(std::make_shared<TypedColumn<Int64Type>>(column));
        break;
    case Type::FLOAT:
        value = std::make_shared<TypedComparator<FloatType>>(std::make_shared<TypedColumn<FloatType>>(column));
        break;
    case Type::DOUBLE:
        value = std::make_shared<TypedComparator<DoubleType>>(std::make_shared<TypedColumn<DoubleType>>(column));
        break;
    case Type::STRING:
        value = std::make_shared<TypedComparator<StringType>>(std::make_shared<TypedColumn<StringType>>(column));
        break;
    default:
        value = nullptr;
        break;
    }

    return value;
}

std::unique_ptr<ValueComparator> Builder::valueComparatorMaker(Type::type type)
{
    std::unique_ptr<ValueComparator> value;
    switch (type) {
    case Type::UINT8:
        value = std::make_unique<TypedValueComparator<UInt8Type>>(TypedValueComparator<UInt8Type>());
        break;
    case Type::INT8:
        value = std::make_unique<TypedValueComparator<Int8Type>>(TypedValueComparator<Int8Type>());
        break;
    case Type::UINT16:
        value = std::make_unique<TypedValueComparator<UInt16Type>>(TypedValueComparator<UInt16Type>());
        break;
    case Type::INT16:
        value = std::make_unique<TypedValueComparator<Int16Type>>(TypedValueComparator<Int16Type>());
        break;
    case Type::UINT32:
        value = std::make_unique<TypedValueComparator<UInt32Type>>(TypedValueComparator<UInt32Type>());
        break;
    case Type::INT32:
        value = std::make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>());
        break;
    case Type::UINT64:
        value = std::make_unique<TypedValueComparator<UInt64Type>>(TypedValueComparator<UInt64Type>());
        break;
    case Type::INT64:
        value = std::make_unique<TypedValueComparator<Int64Type>>(TypedValueComparator<Int64Type>());
        break;
    case Type::FLOAT:
        value = std::make_unique<TypedValueComparator<FloatType>>(TypedValueComparator<FloatType>());
        break;
    case Type::DOUBLE:
        value = std::make_unique<TypedValueComparator<DoubleType>>(TypedValueComparator<DoubleType>());
        break;
    case Type::STRING:
        value = std::make_unique<TypedValueComparator<StringType>>(TypedValueComparator<StringType>());
        break;
    default:
        value = nullptr;
        break;
    }

    return value;
}

std::unique_ptr<Aggregator> Builder::aggregatorMaker(Type::type type, AggType::type agg_type)
{
    std::unique_ptr<Aggregator> value;

    switch (type) {
    case Type::UINT8:
        value = Aggregator::factory<UInt8Type>(agg_type);
        break;
    case Type::INT8:
        value = Aggregator::factory<Int8Type>(agg_type);
        break;
    case Type::UINT16:
        value = Aggregator::factory<UInt16Type>(agg_type);
        break;
    case Type::INT16:
        value = Aggregator::factory<Int16Type>(agg_type);
        break;
    case Type::UINT32:
        value = Aggregator::factory<UInt32Type>(agg_type);
        break;
    case Type::INT32:
        value = Aggregator::factory<Int32Type>(agg_type);
        break;
    case Type::UINT64:
        value = Aggregator::factory<UInt64Type>(agg_type);
        break;
    case Type::INT64:
        value = Aggregator::factory<Int64Type>(agg_type);
        break;
    case Type::FLOAT:
        value = Aggregator::factory<FloatType>(agg_type);
        break;
    case Type::DOUBLE:
        value = Aggregator::factory<DoubleType>(agg_type);
        break;
    case Type::STRING:
        value = Aggregator::factory<StringType>(agg_type);
        break;
    default:
        value = nullptr;
        break;
    }

    return value;
}

void Builder::valueMaker(Type::type type)
{
    Value* value = nullptr;
    switch (type) {
    case Type::UINT8:
        value = new TypedValue<UInt8Type>();
        break;
    case Type::INT8:
        value = new TypedValue<Int8Type>();
        break;
    case Type::UINT16:
        value = new TypedValue<UInt16Type>();
        break;
    case Type::INT16:
        value = new TypedValue<Int16Type>();
        break;
    case Type::UINT32:
        value = new TypedValue<UInt32Type>();
        break;
    case Type::INT32:
        value = new TypedValue<Int32Type>();
        break;
    case Type::UINT64:
        value = new TypedValue<UInt64Type>();
        break;
    case Type::INT64:
        value = new TypedValue<Int64Type>();
        break;
    case Type::FLOAT:
        value = new TypedValue<FloatType>();
        break;
    case Type::DOUBLE:
        value = new TypedValue<DoubleType>();
        break;
    case Type::STRING:
        value = new TypedValue<StringType>();
        break;
    default:
        value = nullptr;
        break;
    }

    _helper.push_back(value);
}

#endif // BUILDER_H
