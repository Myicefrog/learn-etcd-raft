#ifndef __LOG_UNSTABLE_H__
#define __LOG_UNSTABLE_H__

#include "raft.pb.h"
#include "logger.h"
#include "util.h"

using namesapce raftpb;

struct unstable 
{
    unstable() : snapshot_(NULL) 
    {
    }

    // the incoming unstable snapshot, if any.
    Snapshot* snapshot_;

    // all entries that have not yet been written to storage.
    EntryVec entries_;
    uint64_t offset_;
    Logger *logger_;

    bool maybeFirstIndex(uint64_t *first);

    bool  maybeLastIndex(uint64_t* last);

    bool maybeTerm(uint64_t i, uint64_t *term);

    void stableTo(uint64_t i, uint64_t t);

    void stableSnapTo(uint64_t i);

    void restore(const Snapshot& snapshot);

    void truncateAndAppend(const EntryVec& entries);

    void slice(uint64_t lo, uint64_t hi, EntryVec *entries);

    void mustCheckOutOfBounds(uint64_t lo, uint64_t hi);
};

#endif
