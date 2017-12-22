#include "groupby.h"

void GroupBy::run()
{
    uint64_t size = 0;
    for(uint32_t pos : _sorting)
    {
        if (size == 0)
        {
            for(uint64_t spos = 0; spos < _source.size(); spos++)
            {
                Aggregator* agg = _aggregators[spos].get();

                agg->input(_source[spos]->getValue(pos));

                _destination[spos]->putValue(agg->output());

                agg->reset();
            }
            size++;
        } else {
            bool found = false;

            for(uint32_t gpos : _group_cols)
            {
                ValueComparator* comp = _group[gpos].get();
                comp->setValue(_source[gpos]->getValue(pos));
                if(comp->operator !=(_destination[gpos]->getValue(size - 1)) == true)
                {
                    found = false;
                    break;
                } else {
                    found = true;
                }
            }

            if (found == false)
            {
                for(uint64_t spos = 0; spos < _source.size(); spos++)
                {
                    _destination[spos]->putValue(_source[spos]->getValue(pos));
                }
                size++;
            }
        }
    }
}
