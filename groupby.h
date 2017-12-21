#ifndef GROUPGY_H
#define GROUPGY_H

#include <vector>
#include <memory>

#include "column.h"
#include "comparator.h"

class GroupBy
{
private:
    std::vector<std::unique_ptr<Column>>& _source;
    std::vector<std::unique_ptr<Column>>& _destination;
    std::vector<uint32_t>& _sorting;
    std::vector<std::unique_ptr<ValueComparator>>& _group;
    std::vector<uint32_t>& _cols;
public:
    GroupBy(
            std::vector<std::unique_ptr<Column>>& columns,
            std::vector<std::unique_ptr<Column>>& destination,
            std::vector<uint32_t>& sorting,
            std::vector<std::unique_ptr<ValueComparator>>& group,
            std::vector<uint32_t>& cols
            ):_source(columns), _destination(destination),_sorting(sorting),_group(group),_cols(cols){}
    void run();
};

#endif // GROUPGY_H
