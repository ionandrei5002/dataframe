#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <functional>

#include "types.h"
#include "bytebuffer.h"
#include "column.h"
#include "value.h"
#include "comparator.h"

using namespace std;

void split(vector<string>& results, string const& original, char separator);

int main()
{
    fstream input("/home/andrei/Desktop/data.csv");
    string line;

    vector<string> piece;
    vector<uint32_t> sorting;

    vector<unique_ptr<Column>> columns;

    columns.push_back(Column::factory(Type::UINT8));
    columns.push_back(Column::factory(Type::INT8));
    columns.push_back(Column::factory(Type::UINT16));
    columns.push_back(Column::factory(Type::INT16));
    columns.push_back(Column::factory(Type::UINT32));
    columns.push_back(Column::factory(Type::INT32));
    columns.push_back(Column::factory(Type::UINT64));
    columns.push_back(Column::factory(Type::INT64));
    columns.push_back(Column::factory(Type::FLOAT));
    columns.push_back(Column::factory(Type::DOUBLE));
    columns.push_back(Column::factory(Type::STRING));

    uint32_t counter = 0;
    while(getline(input, line))
    {
        split(piece, line, ';');
        for(size_t i = 0; i < piece.size(); i++)
        {
            string* value = &piece.at(i);
            columns.at(i)->putValue(value->data(), value->size());
        }

        piece.clear();
        sorting.push_back(counter);
        counter++;
//        if(counter == 100000)
//            break;
    }

    vector<unique_ptr<Comparator>> comparators;
    {
        comparators.push_back(make_unique<TypedComparator<UInt16Type>>(make_shared<TypedColumn<UInt16Type>>(columns[2])));

        comparators.push_back(make_unique<TypedComparator<Int16Type>>(make_shared<TypedColumn<Int16Type>>(columns[3])));

        comparators.push_back(make_unique<TypedComparator<UInt32Type>>(make_shared<TypedColumn<UInt32Type>>(columns[4])));

        comparators.push_back(make_unique<TypedComparator<Int32Type>>(make_shared<TypedColumn<Int32Type>>(columns[5])));
    }

    for(auto it = comparators.begin(); it != comparators.end(); ++it)
        sort(sorting.begin(), sorting.end(), (*it)->_sorter);

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

    {
        uint64_t size = columns[0]->nb_elements;
        for(uint64_t i = 0; i < size; i++)
        {
            for(uint64_t j = 0; j < columns.size() - 1; j++)
            {
                std::unique_ptr<Value> value = columns[j]->getValue(i);
                value->print(cout);
                cout << ",";
            }
            std::unique_ptr<Value> value = columns[columns.size() - 1]->getValue(i);
            value->print(cout);
            cout << endl;
        }
    }

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
