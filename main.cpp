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

using namespace std;

void split(vector<string>& results, string const& original, char separator);

int main()
{
    chrono::time_point<std::chrono::high_resolution_clock> start, end;

    fstream input("/home/andrei/Desktop/dataset.csv");
    string line;

    vector<string> piece;
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
                    string* value = &piece.at(i);
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

    vector<shared_ptr<Comparator>> comparators;
    {
        comparators.push_back(make_shared<TypedComparator<Int32Type>>(make_shared<TypedColumn<Int32Type>>(columns[3])));
        comparators.push_back(make_shared<TypedComparator<Int32Type>>(make_shared<TypedColumn<Int32Type>>(columns[4])));
    }

    {
        start = chrono::high_resolution_clock::now();

        sort(sorting.begin(), sorting.end(), Sorter(comparators));

        end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed_time = end - start;

        cout << "sort duration = " << elapsed_time.count() << "s" << std::endl;
    }

    input.close();

    {
        uint64_t total = 0;
        for(uint64_t i = 0; i < columns.size(); i++)
        {
            uint64_t colsize = columns[i]->_column.size();
            uint64_t idxsize = columns[i]->_position.size() * 8;
            cout << i << " - " << colsize + idxsize << " byte" << endl;
            total += colsize + idxsize;
        }
        cout << "total size " << total << " bytes" << endl;

        for(uint64_t i = 0; i < columns.size(); i++)
        {
            uint64_t size = columns[i]->nb_elements;
            cout << size << endl;
        }
    }

    ofstream out("/home/andrei/Desktop/sorted.csv");

    {
        start = chrono::high_resolution_clock::now();

        uint64_t size = sorting.size();
        for(uint64_t i = 0; i < size; i++)
        {
            for(uint64_t j = 0; j < columns.size() - 1; j++)
            {
                Value* value = columns[j]->getValue(sorting[i]);
                out << (*value) << ",";
            }
            Value* value = columns[columns.size() - 1]->getValue(sorting[i]);
            out << (*value) << endl;
        }

        end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed_time = end - start;

        cout << "write sort duration = " << elapsed_time.count() << "s" << std::endl;
    }

    out.close();

//    vector<unique_ptr<Column>> destination;

//    destination.push_back(Column::factory(Type::STRING));
//    destination.push_back(Column::factory(Type::STRING));
//    destination.push_back(Column::factory(Type::INT32));
//    destination.push_back(Column::factory(Type::INT32));
//    destination.push_back(Column::factory(Type::INT32));
//    destination.push_back(Column::factory(Type::UINT64));
//    destination.push_back(Column::factory(Type::INT32));
//    destination.push_back(Column::factory(Type::INT32));
//    destination.push_back(Column::factory(Type::INT32));
//    destination.push_back(Column::factory(Type::INT32));
//    destination.push_back(Column::factory(Type::STRING));
//    destination.push_back(Column::factory(Type::STRING));
//    destination.push_back(Column::factory(Type::STRING));
//    destination.push_back(Column::factory(Type::INT32));
//    destination.push_back(Column::factory(Type::STRING));
//    destination.push_back(Column::factory(Type::STRING));
//    destination.push_back(Column::factory(Type::STRING));
//    destination.push_back(Column::factory(Type::INT32));

//    vector<unique_ptr<ValueComparator>> group;
//    group.push_back(make_unique<TypedValueComparator<StringType>>(TypedValueComparator<StringType>()));
//    group.push_back(make_unique<TypedValueComparator<StringType>>(TypedValueComparator<StringType>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));
//    group.push_back(make_unique<TypedValueComparator<StringType>>(TypedValueComparator<StringType>()));
//    group.push_back(make_unique<TypedValueComparator<StringType>>(TypedValueComparator<StringType>()));
//    group.push_back(make_unique<TypedValueComparator<StringType>>(TypedValueComparator<StringType>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));
//    group.push_back(make_unique<TypedValueComparator<StringType>>(TypedValueComparator<StringType>()));
//    group.push_back(make_unique<TypedValueComparator<StringType>>(TypedValueComparator<StringType>()));
//    group.push_back(make_unique<TypedValueComparator<StringType>>(TypedValueComparator<StringType>()));
//    group.push_back(make_unique<TypedValueComparator<Int32Type>>(TypedValueComparator<Int32Type>()));

//    vector<uint32_t> group_cols;
//    group_cols.push_back(3);
//    group_cols.push_back(4);

//    vector<unique_ptr<Aggregator>> aggregators;
//    aggregators.push_back(make_unique<None<StringType>>(None<StringType>()));
//    aggregators.push_back(make_unique<None<StringType>>(None<StringType>()));
//    aggregators.push_back(make_unique<None<Int32Type>>(None<Int32Type>()));
//    aggregators.push_back(make_unique<None<Int32Type>>(None<Int32Type>()));
//    aggregators.push_back(make_unique<None<Int32Type>>(None<Int32Type>()));
//    aggregators.push_back(make_unique<Sum<Int32Type>>(Sum<Int32Type>()));
//    aggregators.push_back(make_unique<None<Int32Type>>(None<Int32Type>()));
//    aggregators.push_back(make_unique<None<Int32Type>>(None<Int32Type>()));
//    aggregators.push_back(make_unique<None<Int32Type>>(None<Int32Type>()));
//    aggregators.push_back(make_unique<None<Int32Type>>(None<Int32Type>()));
//    aggregators.push_back(make_unique<None<StringType>>(None<StringType>()));
//    aggregators.push_back(make_unique<None<StringType>>(None<StringType>()));
//    aggregators.push_back(make_unique<None<StringType>>(None<StringType>()));
//    aggregators.push_back(make_unique<None<Int32Type>>(None<Int32Type>()));
//    aggregators.push_back(make_unique<None<StringType>>(None<StringType>()));
//    aggregators.push_back(make_unique<None<StringType>>(None<StringType>()));
//    aggregators.push_back(make_unique<None<StringType>>(None<StringType>()));
//    aggregators.push_back(make_unique<None<Int32Type>>(None<Int32Type>()));

//    vector<uint32_t> agg_cols;
//    agg_cols.push_back(5);

//    {
//        start = chrono::high_resolution_clock::now();
//        GroupBy groupby(columns, destination, sorting, group, group_cols, aggregators, agg_cols);
//        groupby.run();
//        end = chrono::high_resolution_clock::now();
//        chrono::duration<double> elapsed_time = end - start;

//        cout << "group duration = " << elapsed_time.count() << "s" << std::endl;
//    }

//    {
//        uint64_t total = 0;
//        for(uint64_t i = 0; i < destination.size(); i++)
//        {
//            uint64_t colsize = destination[i]->_column.size();
//            uint64_t idxsize = destination[i]->_position.size() * 8;
//            cout << i << " - " << colsize + idxsize << " byte" << endl;
//            total += colsize + idxsize;
//        }
//        cout << "total size " << total << " bytes" << endl;

//        for(uint64_t i = 0; i < destination.size(); i++)
//        {
//            uint64_t size = destination[i]->nb_elements;
//            cout << size << endl;
//        }
//    }

//    ofstream outgroup("/home/andrei/Desktop/group.csv");

//    {
//        start = chrono::high_resolution_clock::now();

//        uint64_t size = destination[0]->nb_elements;
//        for(uint64_t i = 0; i < size; i++)
//        {
//            for(uint64_t j = 0; j < destination.size() - 1; j++)
//            {
//                Value* value = destination[j]->getValue(i);
//                outgroup << (*value) << ",";
//            }
//            Value* value = destination[destination.size() - 1]->getValue(i);
//            outgroup << (*value) << endl;
//        }

//        end = chrono::high_resolution_clock::now();
//        chrono::duration<double> elapsed_time = end - start;

//        cout << "write group duration = " << elapsed_time.count() << "s" << std::endl;
//    }

//    outgroup.close();

    return 0;
}

void split(vector<string>& results, string const& original, char separator)
{
    string::const_iterator start = original.begin();
    string::const_iterator end = original.end();
    string::const_iterator next = find(start, end, separator);
    while(next != end && *start == '"' && *(next-1) != '"')
    {
        next = find(next + 1, end, separator);
    }
    while (next != end) {

        string str(start, next);
        results.emplace_back(str);
        start = next + 1;
        next = find(start, end, separator);
        while(*start == '"' && *(next-1) != '"' && next != end)
        {
            next = find(next + 1, end, separator);
        }
    }
    string str(start, next);
    results.emplace_back(str);
}
