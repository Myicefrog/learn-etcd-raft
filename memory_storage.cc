#include "memory_storage.h"

MemoryStorage::MemoryStorage(Logger *logger) 
    : snapShot_(new Snapshot())
    , logger_(logger) 
{
    entries_.push_back(Entry());
}

MemoryStorage::~MemoryStorage() 
{
    delete snapShot_;
}

int MemoryStorage::InitialState(HardState *hs, ConfState *cs) 
{
    *hs = hardState_;
    *cs = snapShot_->metadata().conf_state();
    return OK;
}

int MemoryStorage::SetHardState(const HardState& hs) 
{
    Mutex mutex(&locker_);
    hardState_ = hs;
    return OK;
}

int MemoryStorage::Entries(uint64_t lo, uint64_t hi, uint64_t maxSize, vector<Entry> *entries)
{

    Mutex mutex(&locker_);
    uint64_t offset = entries_[0].index();
    if (lo <= offset) 
    {
        return ErrCompacted;
    }

    if (hi > lastIndex() + 1) 
    {
        logger_->Panicf(__FILE__, __LINE__, "entries hi(%d) is out of bound lastindex(%d)", hi, lastIndex());
  
    }
  // only contains dummy entries.
    if (entries_.size() == 1) 
    {
        return ErrUnavailable;
    }
    int i;
    for (i = lo - offset; i < hi - offset; ++i) 
    {
       entries->push_back(entries_[i]);
    }
    limitSize(entries, maxSize);
    return OK;

}

int MemoryStorage::Term(uint64_t i, uint64_t *term) 
{
    Mutex mutex(&locker_);
    *term = 0;
    uint64_t offset = entries_[0].index();
    if (i < offset) 
    {
       return ErrCompacted;
    }
    if (i - offset >= entries_.size()) 
    {
       return ErrUnavailable;
    }
    *term = entries_[i - offset].term();
    return OK;
}

int MemoryStorage::LastIndex(uint64_t *index) 
{
    Mutex mutex(&locker_);
    *index = lastIndex(); 
    return OK;
}


uint64_t MemoryStorage::lastIndex() 
{
  return entries_[0].index() + entries_.size() - 1;
}

int MemoryStorage::FirstIndex(uint64_t *index) 
{
    Mutex mutex(&locker_);
    *index = firstIndex();
    return OK;
}

uint64_t MemoryStorage::firstIndex() 
{
  return entries_[0].index() + 1;
}

/*
Snapshot*  MemoryStorage::Snapshot() 
{
  Mutex mutex(&locker_);
  return snapShot_;
}
*/

int MemoryStorage::ApplySnapshot(const Snapshot& snapshot) 
{
    Mutex mutex(&locker_);

  //handle check for old snapshot being applied
    uint64_t index = snapShot_->metadata().index();
    uint64_t snapIndex = snapshot.metadata().index();
    if (index >= snapIndex) 
    {
        return ErrSnapOutOfDate;
    }

    snapShot_->CopyFrom(snapshot);
    entries_.clear();
    Entry entry;
    entry.set_index(snapshot.metadata().index());
    entry.set_term(snapshot.metadata().term());
    entries_.push_back(entry);
    return OK;
}

int MemoryStorage::CreateSnapshot(uint64_t i, ConfState *cs, const string& data, Snapshot *ss) 
{
    Mutex mutex(&locker_);

    if (i <= snapShot_->metadata().index()) 
    {
        return ErrSnapOutOfDate;
    }

    uint64_t offset = entries_[0].index();
    if (i > lastIndex()) 
    {
        logger_->Panicf(__FILE__, __LINE__, "snapshot %llu is out of bound lastindex(%llu)", i, lastIndex());
    }

    snapShot_->mutable_metadata()->set_index(i);  
    snapShot_->mutable_metadata()->set_term(entries_[i - offset].term());  
    if (cs != NULL) 
    {
        *(snapShot_->mutable_metadata()->mutable_conf_state()) = *cs;
    }
    snapShot_->set_data(data);
    if (ss != NULL) 
    {
        *ss = *snapShot_;
    }

    return OK;
}

int MemoryStorage::Compact(uint64_t compactIndex) {
    Mutex mutex(&locker_);

    uint64_t offset = entries_[0].index();
    if (compactIndex <= offset)
    {
        return ErrCompacted;
    }
    if (compactIndex > lastIndex()) 
    {
        logger_->Panicf(__FILE__, __LINE__, "compact %llu is out of bound lastindex(%llu)", compactIndex, lastIndex());
    }

    uint64_t i = compactIndex - offset;
    EntryVec entries;
    Entry entry;
    entry.set_index(entries_[i].index());
    entry.set_term(entries_[i].term());
    entries.push_back(entry);
    for (i = i + 1; i < entries_.size(); ++i) 
    {
        entries.push_back(entries_[i]);
    }
    entries_ = entries;
    return OK;
}

int MemoryStorage::Append(const EntryVec& entries) {
    if (entries.empty()) 
    {
        return OK;
    }

    Mutex mutex(&locker_);
    EntryVec appendEntries = entries;

    uint64_t first = firstIndex();
    uint64_t last  = appendEntries[0].index() + appendEntries.size() - 1;

    if (last < first) 
    {
        return OK;
    }

    // truncate compacted entries
    if (first > appendEntries[0].index()) 
    {
        uint64_t index = first - appendEntries[0].index();
        appendEntries.erase(appendEntries.begin(), appendEntries.begin() + index);
    }

    uint64_t offset = appendEntries[0].index() - entries_[0].index();
    if (entries_.size() > offset) 
    {
        entries_.erase(entries_.begin(), entries_.begin() + offset);
        int i;
        for (i = 0; i < appendEntries.size(); ++i) 
        {
            entries_.push_back(appendEntries[i]);
        }
        return OK;
    }

    if (entries_.size() == offset) 
    {
        int i;
        for (i = 0; i < appendEntries.size(); ++i) 
        {
            entries_.push_back(appendEntries[i]);
        }
        return OK;
    }

    logger_->Panicf(__FILE__, __LINE__, "missing log entry [last: %llu, append at: %llu]",
    lastIndex(), appendEntries[0].index());
    return OK;
}

int MemoryStorage::GetSnapshot(Snapshot **snapshot) 
{
    Mutex mutex(&locker_);
    *snapshot = snapShot_;
    return OK;
}
