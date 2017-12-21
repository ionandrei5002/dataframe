#include "groupby.h"

void GroupBy::run()
{
    uint64_t size = 0;
    for(uint32_t pos : _sorting)
    {
        if (size == 0)
        {
            for(uint64_t spos = 0; spos < _columns.size(); spos++)
            {
                _destination[spos]->putValue(_columns[spos]->getValue(pos));
            }
            size++;
        } else {
            bool found = false;

            for(uint32_t gpos : _group)
            {
                if (_columns[gpos]->getValue(pos)->differit(_columns[gpos]->getValue(pos),_destination[gpos]->getValue(size - 1))) {
                    found = false;
                    break;
                } else {
                    found = true;
                }
            }

            if (found == false)
            {
                for(uint64_t spos = 0; spos < _columns.size(); spos++)
                {
                    _destination[spos]->putValue(_columns[spos]->getValue(pos));
                }
                size++;
            }
        }
    }
}
