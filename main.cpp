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

    return 0;
}

void split(vector<string>& results, string const& original, char separator)
{
    string::const_iterator start = original.begin();
    string::const_iterator end = original.end();
    string::const_iterator next = find(start, end, separator);
    while (next != end) {

        string str(start, next);
        results.emplace_back(str);
        start = next + 1;
        next = find(start, end, separator);
    }
}
