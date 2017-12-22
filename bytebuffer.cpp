#include "bytebuffer.h"

bytebuffer::bytebuffer()
{}

bytebuffer::bytebuffer(uint64_t size)
{
    _size = size;
}

bytebuffer::bytebuffer(uint64_t size, char *buffer)
{
    _size = size;
    _buffer = buffer;
}

bytebuffer::bytebuffer(const bytebuffer &ot)
{
    _size = ot._size;
    _buffer = ot._buffer;
}

bytebuffer::bytebuffer(bytebuffer &&ot)
{
    _size = ot._size;
    _buffer = ot._buffer;

    ot._size = 0;
    ot._buffer = nullptr;
}

bytebuffer& bytebuffer::operator =(const bytebuffer& ot)
{
    _size = ot._size;
    _buffer = ot._buffer;

    return *this;
}

bytebuffer& bytebuffer::operator =(bytebuffer&& ot)
{
    _size = ot._size;
    _buffer = ot._buffer;

    ot._size = 0;
    ot._buffer = nullptr;

    return *this;
}

bytebuffer::~bytebuffer()
{
    _buffer = nullptr;
    _size = 0;
}

std::ostream& operator <<(std::ostream& output, const bytebuffer& ot)
{
    for(uint64_t i = 0; i < ot._size; i++)
    {
        output << ot._buffer[i];
    }

    return output;
}

std::istream& operator >>(std::istream& input, bytebuffer& ot)
{
    return input;
}
