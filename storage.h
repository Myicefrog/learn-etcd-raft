#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "raft.pb.h"
#include "util.h"

//using namespace raftpb;

class Storage 
{
public:
    virtual ~Storage() {}

    virtual int InitialState(HardState* hs, ConfState* cs) = 0;
    virtual int Entries(uint64_t lo, uint64_t hi, uint64_t maxSize, vector<Entry> *entries) = 0;
    virtual int Term(uint64_t i,uint64_t *term) = 0;
    virtual int  LastIndex(uint64_t *index) = 0;
    virtual int  FirstIndex(uint64_t *index) = 0;
    //virtual Snapshot* Snapshot() = 0;
    virtual int GetSnapshot(Snapshot **snapshot) = 0;
};

#endif
