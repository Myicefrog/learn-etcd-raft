#ifndef __LOG_H__
#define __LOG_H__

#include "log_unstable.h"
#include "storage.h"
#include "util.h"

struct raftLog
{
    // storage contains all stable entries since the last snapshot.
    Storage *storage_;

    // unstable contains all unstable entries and snapshot.
    // they will be saved into storage.
    unstable unstable_;

    // committed is the highest log position that is known to be in
    // stable storage on a quorum of nodes.
    uint64_t committed_;

    // applied is the highest log position that the application has
    // been instructed to apply to its state machine.
    // Invariant: applied <= committed
    uint64_t applied_;

    Logger *logger_;

    // maxNextEntsSize is the maximum number aggregate byte size of the messages
    // returned from calls to nextEnts.
    uint64_t maxNextEntsSize; 

    raftLog(Storage *, Logger *);
    string String();

    bool maybeAppend(uint64_t index, uint64_t logTerm, 
                   uint64_t committed, const EntryVec& entries, uint64_t *lasti);

    uint64_t append(const EntryVec& entries);

    uint64_t findConflict(const EntryVec& entries);

    void unstableEntries(EntryVec *entries);

    void nextEntries(EntryVec* entries);

    bool hasNextEntries();

    int snapshot(Snapshot **snapshot);

    uint64_t firstIndex();

    uint64_t lastIndex();

    void commitTo(uint64_t tocommit);

    void appliedTo(uint64_t i);

    void stableTo(uint64_t i, uint64_t t);

    void stableSnapTo(uint64_t i);

    uint64_t lastTerm();
    
    int term(uint64_t i, uint64_t *t);

    int entries(uint64_t i, uint64_t maxSize, EntryVec *entries);

    void allEntries(EntryVec *entries);

    bool isUpToDate(uint64_t lasti, uint64_t term);

    bool matchTerm(uint64_t i, uint64_t term);

    bool maybeCommit(uint64_t maxIndex, uint64_t term);

    void restore(const Snapshot& snapshot);

    int slice(uint64_t lo, uint64_t hi, uint64_t maxSize, EntryVec* entries);

    int mustCheckOutOfBounds(uint64_t lo, uint64_t hi);


    uint64_t zeroTermOnErrCompacted(uint64_t t, int err);

};

extern raftLog* newLog(Storage *storage, Logger *logger);

#endif
