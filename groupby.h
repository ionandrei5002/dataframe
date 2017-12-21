#ifndef GROUPGY_H
#define GROUPGY_H

#include <vector>
#include <memory>

#include "column.h"

class GroupBy
{
private:
    std::vector<std::unique_ptr<Column>>& _columns;
    std::vector<std::unique_ptr<Column>>& _destination;
    std::vector<uint32_t>& _sorting;
    std::vector<uint32_t>& _group;
public:
    GroupBy(
            std::vector<std::unique_ptr<Column>>& columns,
            std::vector<std::unique_ptr<Column>>& destination,
            std::vector<uint32_t>& sorting,
            std::vector<uint32_t>& group
            ):_columns(columns), _destination(destination),_sorting(sorting),_group(group){}
    void run();
};

#endif // GROUPGY_H
