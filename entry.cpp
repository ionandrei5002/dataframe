#include "entry.h"

template<typename Type>
typedentry<Type>::typedentry(std::string& data)
{
    _buffer = bytebuffer(data.size(), reinterpret_cast<uint8_t*>(const_cast<char*>(data.data())));
}

template<typename Type>
void typedentry<Type>::print(std::ostream& output) const
{
    output << _buffer;
}

std::unique_ptr<entry> entry::factory(Type::type type)
{
    std::unique_ptr<entry> value;

    switch (type)
    {
        case Type::UINT8:
            value = std::make_unique<typedentry<UInt8Type>>(typedentry<UInt8Type>());
        default:
            value = nullptr;
    };

    return value;
}

template class typedentry<UInt8Type>;
template class typedentry<Int8Type>;
template class typedentry<UInt32Type>;
template class typedentry<Int32Type>;
template class typedentry<UInt64Type>;
template class typedentry<Int64Type>;
template class typedentry<StringType>;
