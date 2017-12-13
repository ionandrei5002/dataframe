#include "bytebuffer.h"

bytebuffer::bytebuffer()
{}

bytebuffer::bytebuffer(uint64_t size)
{
    _size = size;
    _buffer = new uint8_t[_size]{0};
}

bytebuffer::bytebuffer(uint64_t size, uint8_t *buffer)
{
    _size = size;
    _buffer = new uint8_t[_size]{0};

    std::memcpy(_buffer, buffer, _size);
}

bytebuffer::bytebuffer(const bytebuffer &ot)
{
    _size = ot._size;
    _buffer = new uint8_t[_size]{0};

    std::memcpy(_buffer, ot._buffer, _size);
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
    _buffer = new uint8_t[_size]{0};

    std::memcpy(_buffer, ot._buffer, _size);

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
    delete[] _buffer;
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
