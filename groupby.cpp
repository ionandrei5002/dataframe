#include "groupby.h"

void GroupBy::helper()
{
    _helper.push_back(new TypedValue<StringType>());
    _helper.push_back(new TypedValue<StringType>());
    _helper.push_back(new TypedValue<Int32Type>());
    _helper.push_back(new TypedValue<Int32Type>());
    _helper.push_back(new TypedValue<Int32Type>());
    _helper.push_back(new TypedValue<Int32Type>());
    _helper.push_back(new TypedValue<Int32Type>());
    _helper.push_back(new TypedValue<Int32Type>());
    _helper.push_back(new TypedValue<Int32Type>());
    _helper.push_back(new TypedValue<Int32Type>());
    _helper.push_back(new TypedValue<StringType>());
    _helper.push_back(new TypedValue<StringType>());
    _helper.push_back(new TypedValue<StringType>());
    _helper.push_back(new TypedValue<Int32Type>());
    _helper.push_back(new TypedValue<StringType>());
    _helper.push_back(new TypedValue<StringType>());
    _helper.push_back(new TypedValue<StringType>());
    _helper.push_back(new TypedValue<Int32Type>());
}

void GroupBy::run()
{
    this->helper();

    uint64_t size = 0;
    bool found = false;

    for(uint32_t pos : _sorting)
    {
        if (size == 0)
        {
            for(uint64_t spos = 0; spos < _source.size(); spos++)
            {
                Aggregator* agg = _aggregators[spos].get();

                agg->input(_source[spos]->getValue(pos));

                _helper[spos]->setValue(_source[spos]->getValue(pos)->getValue());
            }
            size++;
        } else {
            for(uint32_t gpos : _group_cols)
            {
                ValueComparator* comp = _group[gpos].get();
                comp->setValue(_source[gpos]->getValue(pos));

                if(comp->operator !=(_helper[gpos]) == true)
                {
                    found = false;
                    break;
                } else {
                    found = true;
                }
            }

            if (found == true)
            {
                for(uint64_t spos = 0; spos < _source.size(); spos++)
                {
                    Aggregator* agg = _aggregators[spos].get();

                    agg->input(_source[spos]->getValue(pos));
                }
            }

            if (found == false)
            {
                for(uint64_t spos = 0; spos < _source.size(); spos++)
                {
                    Aggregator* agg = _aggregators[spos].get();

                    _destination[spos]->putValue(agg->output());

                    agg->reset();

                    agg->input(_source[spos]->getValue(pos));

                    _helper[spos]->setValue(_source[spos]->getValue(pos)->getValue());
                }
                size++;
            }
        }
    }

    if (found == true)
    {
        for(uint64_t spos = 0; spos < _source.size(); spos++)
        {
            Aggregator* agg = _aggregators[spos].get();

            _destination[spos]->putValue(agg->output());

            agg->reset();
        }
    }
}
