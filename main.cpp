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

using namespace std;

void split(vector<string>& results, string const& original, char separator);

int main()
{
    chrono::time_point<std::chrono::high_resolution_clock> start, end;

    fstream input("/home/andrei/Desktop/oldies/desktop4/waa_android.csv");
    string line;

    vector<string> piece;
    vector<uint32_t> sorting;

    vector<unique_ptr<Column>> columns;

    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));
    columns.push_back(Column::factory(Type::STRING));

    uint32_t counter = 0;
    {
        start = chrono::high_resolution_clock::now();

        while(getline(input, line))
        {
            split(piece, line, ',');
            for(size_t i = 0; i < piece.size(); i++)
            {
                string* value = &piece.at(i);
                columns.at(i)->putValue(value->data(), value->size());
            }

            piece.clear();
            sorting.push_back(counter);
            counter++;
//            if (counter == 100000)
//            {
//                break;
//            }
        }

        end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed_time = end - start;

        cout << "read duration = " << elapsed_time.count() << "s" << std::endl;
    }

    vector<shared_ptr<Comparator>> comparators;
    {
        comparators.push_back(make_shared<TypedComparator<StringType>>(make_shared<TypedColumn<StringType>>(columns[0])));
        comparators.push_back(make_shared<TypedComparator<StringType>>(make_shared<TypedColumn<StringType>>(columns[1])));
        comparators.push_back(make_shared<TypedComparator<StringType>>(make_shared<TypedColumn<StringType>>(columns[2])));
        comparators.push_back(make_shared<TypedComparator<StringType>>(make_shared<TypedColumn<StringType>>(columns[3])));
        comparators.push_back(make_shared<TypedComparator<StringType>>(make_shared<TypedColumn<StringType>>(columns[10])));
    }

    {
        start = chrono::high_resolution_clock::now();

        sort(sorting.begin(), sorting.end(), Sorter(comparators));

        end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed_time = end - start;

        cout << "sort duration = " << elapsed_time.count() << "s" << std::endl;
    }

    input.close();
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

    ofstream out("/home/andrei/Desktop/sorted.csv");

    {
        start = chrono::high_resolution_clock::now();

        uint64_t size = sorting.size();
        for(uint64_t i = 0; i < size; i++)
        {
            for(uint64_t j = 0; j < columns.size() - 1; j++)
            {
                unique_ptr<Value> value = columns[j]->getValue(sorting[i]);
                value->print(out);
                out << ",";
            }
            unique_ptr<Value> value = columns[columns.size() - 1]->getValue(sorting[i]);
            value->print(out);
            out << endl;
        }

        end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed_time = end - start;

        cout << "write duration = " << elapsed_time.count() << "s" << std::endl;
    }

    out.close();

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
