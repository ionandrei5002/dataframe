#ifndef GROUPGY_H
#define GROUPGY_H

#include <vector>
#include <memory>

#include "column.h"
#include "comparator.h"
#include "aggregator.h"

class GroupBy
{
private:    
    std::vector<std::unique_ptr<Column>>& _source;
    std::vector<std::unique_ptr<Column>>& _destination;
    std::vector<uint32_t>& _sorting;
    std::vector<std::unique_ptr<ValueComparator>>& _group;
    std::vector<uint32_t>& _group_cols;
    std::vector<std::unique_ptr<Aggregator>>& _aggregators;
    std::vector<Value*> _helper;
public:
    GroupBy(
            std::vector<std::unique_ptr<Column>>& columns,
            std::vector<std::unique_ptr<Column>>& destination,
            std::vector<uint32_t>& sorting,
            std::vector<std::unique_ptr<ValueComparator>>& group,
            std::vector<uint32_t>& group_cols,
            std::vector<std::unique_ptr<Aggregator>>& aggregators,
            std::vector<Value*> helper
            ):
        _source(columns),
        _destination(destination),
        _sorting(sorting),
        _group(group),
        _group_cols(group_cols),
        _aggregators(aggregators),
        _helper(helper) {}
    ~GroupBy() {
        for(uint64_t i = 0; i < _helper.size(); i++)
            delete _helper[i];
    }
    void run();
};

#endif // GROUPGY_H
