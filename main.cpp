#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "types.h"
#include "bytebuffer.h"
#include "column.h"
#include "value.h"

using namespace std;

void split(vector<string>& results, string const& original, char separator);

int main()
{
    fstream input("/home/andrei/Desktop/oldies/desktop4/waa_android.csv");
    string line;

    vector<string> piece;

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

    while(getline(input, line))
    {
        split(piece, line, ',');
        for(size_t i = 0; i < piece.size(); i++)
        {
            string* value = &piece.at(i);
            columns.at(i)->putValue(value->data(), value->size());
        }

        piece.clear();
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

    uint64_t size = columns[0]->nb_elements;
    for(uint64_t i = 0; i < size; i++)
    {
        for(uint64_t j = 0; j < columns.size() - 1; j++)
        {
            std::unique_ptr<Value> value = columns[j]->getValue(i);
            cout << value->getValue() << ",";
        }
        std::unique_ptr<Value> value = columns[columns.size() - 1]->getValue(i);
        cout << value->getValue() << endl;
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
