#ifndef __MEMORY_STORAGE_H__
#define __MEMORY_STORAGE_H__

#include "raft.pb.h"
#include "util.h"
#include "mutex.h"
#include "log.h"
#include "storage.h"


class MemoryStorage : public Storage
{
public: 
    
    MemoryStorage(Logger *logger);
    ~MemoryStorage();

    virtual int InitialState(HardState* hs, ConfState* cs);
    virtual int Entries(uint64_t lo, uint64_t hi, uint64_t maxSize, vector<Entry> *entries)
    virtual uint64_t Term(uint64_t i);
    virtual uint64_t LastIndex();
    virtual uint64_t FirstIndex();
    virtual Snapshot* Snapshot();

    int SetHardState(const HardState& );
    int ApplySnapshot(const Snapshot& snapshot);
    int CreateSnapshot(uint64_t i, ConfState *cs, const string& data, Snapshot *ss);
    int Compact(uint64_t compactIndex);
    int Append(const EntryVec& entries);

    //int GetSnapshot(Snapshot **snapshot);

private:

     uint64_t firstIndex();
     uint64_t lastIndex();
public:

    HardState hardState_;
    Snapshot  *snapShot_;
    EntryVec entries_;

    Locker locker_;
    Logger *logger_;
};


#endif
