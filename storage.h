#ifndef __STORAGE_H__
#define __STORAGE_H__


class Storage 
{
public:
    virtual ~Storage() {}

    virtual int InitialState(HardState* hs, ConfState* cs) = 0;
    virtual int Entries(uint64_t lo, uint64_t hi, uint64_t maxSize, vector<Entry> *entries) = 0;
    virtual uint64_t Term(uint64_t i) = 0;
    virtual uint64_t LastIndex() = 0;
    virtual uint64_t FirstIndex() = 0;
    virtual Snapshot* Snapshot() = 0;
};

#endif;
