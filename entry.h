#ifndef ENTRY_H
#define ENTRY_H

#include "bytebuffer.h"
#include "types.h"

class entry
{
public:
    virtual ~entry() {}
    virtual void print(std::ostream&) const = 0;
    static std::unique_ptr<entry> factory(Type::type type);
};

template<typename Type>
class typedentry : public entry
{
private:
    bytebuffer _buffer;
public:
    typedentry(){}
    typedentry(std::string& data);
    void print(std::ostream& output) const override;
};

#endif // ENTRY_H
