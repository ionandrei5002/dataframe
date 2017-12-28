#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>

#include "types.h"
#include "bytebuffer.h"
#include "column.h"
#include "value.h"
#include "comparator.h"
#include "groupby.h"
#include "aggregator.h"

#include "builder.h"

#include <csignal>
#include <experimental/string_view>

using namespace std;

void split(vector<experimental::string_view>& results, string const& original, char separator);

int main()
{
    chrono::time_point<std::chrono::high_resolution_clock> start, end;

    fstream input("/home/andrei/Desktop/dataset.csv");
    string line;
    line.reserve(1024 * 1024);

    vector<experimental::string_view> piece;

    vector<uint32_t> sorting;

    vector<unique_ptr<Column>> columns;

    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::INT32));

    piece.reserve(columns.size());

    uint32_t counter = 0;
    {
        start = chrono::high_resolution_clock::now();

        getline(input, line);
        while(getline(input, line))
        {
            split(piece, line, ',');
            for(size_t i = 0; i < piece.size(); i++)
            {
                try {
                    experimental::string_view* value = &piece.at(i);
                    columns.at(i)->putValue(value->data(), value->size());
                } catch(exception& ex)
                {
                    cout << __FILE__ << __LINE__ << ex.what() << " " << line << endl;
                    abort();
                }
            }

            piece.clear();
            sorting.push_back(counter);
            counter++;
        }

        end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed_time = end - start;

        cout << "read duration = " << elapsed_time.count() << "s" << std::endl;
    }

    {
        vector<unique_ptr<Column>> dest;
        Builder builder(columns);
        dest = builder.applySortColumn(3).applySortColumn(4)
                .applyGroupByColumn(3).applyGroupByColumn(4)
                .applyAggregateColumn(5, AggType::SUM)
                .applyAggregateColumn(6, AggType::SUM)
                .applyAggregateColumn(7, AggType::DISTINCTC_COUNT)
                .applyAggregateColumn(16, AggType::DISTINCTC_COUNT)
                .run();

        {
            uint64_t total = 0;
            for(uint64_t i = 0; i < dest.size(); i++)
            {
                uint64_t colsize = dest[i]->_column.size();
                uint64_t idxsize = dest[i]->_position.size() * 8;
                cout << i << " - " << colsize + idxsize << " byte" << endl;
                total += colsize + idxsize;
            }
            cout << "total size " << total << " bytes" << endl;
        }

        ofstream outgroup("/home/andrei/Desktop/group.csv");

        {
            start = chrono::high_resolution_clock::now();

            uint64_t size = dest[0]->nb_elements;
            for(uint64_t i = 0; i < size; i++)
            {
                for(uint64_t j = 0; j < dest.size() - 1; j++)
                {
                    Value* value = dest[j]->getValue(i);
                    outgroup << (*value) << ",";
                }
                Value* value = dest[dest.size() - 1]->getValue(i);
                outgroup << (*value) << endl;
            }

            end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed_time = end - start;

            cout << "write group duration = " << elapsed_time.count() << "s" << std::endl;
        }

        outgroup.close();
    }

    return 0;
}

void split(vector<experimental::string_view>& results, string const& original, char separator)
{
    string::const_iterator start = original.begin();
    string::const_iterator end = original.end();
    string::const_iterator next = find(start, end, separator);
    while(next != end && *start == '"' && *(next-1) != '"')
    {
        next = find(next + 1, end, separator);
    }
    while (next != end) {
        experimental::string_view str(start.operator ->(), static_cast<uint64_t>(next.operator ->() - start.operator ->()));
        results.push_back(str);

        start = next + 1;
        next = find(start, end, separator);
        while(*start == '"' && *(next-1) != '"' && next != end)
        {
            next = find(next + 1, end, separator);
        }
    }

    experimental::string_view str(start.operator ->(), static_cast<uint64_t>(next.operator ->() - start.operator ->()));
    results.push_back(str);
}
