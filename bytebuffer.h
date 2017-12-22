#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include<memory>
#include<iostream>
#include<cstring>

class bytebuffer
{
public:
    uint64_t _size = 0;
    char* _buffer = nullptr;
public:
    bytebuffer(); // default ctor
    bytebuffer(uint64_t size); // zero init ctor
    bytebuffer(uint64_t size, char* buffer); // data copy init ctor
    bytebuffer(const bytebuffer& ot); // copy ctor
    bytebuffer(bytebuffer&& ot); // move ctor
    bytebuffer& operator =(const bytebuffer& ot); // copy operator
    bytebuffer& operator =(bytebuffer&& ot); // move operator
    ~bytebuffer(); //dtor
    friend std::ostream &operator <<(std::ostream& output, const bytebuffer& ot); // output operator
    friend std::istream &operator >>(std::istream& input, bytebuffer& ot); // input operator
};

#endif // BYTEBUFFER_H
