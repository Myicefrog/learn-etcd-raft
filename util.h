#ifndef __UTIL_H__
#define __UTIL_H__

#include "raft.pb.h"
#include <vector>
#include <cstdint>
#include <climits>

using namespace std;
using namespace raftpb;

typedef vector<Entry> EntryVec;
const static uint64_t None = 0;
const static uint64_t noLimit = ULONG_MAX;

enum ReadOnlyOption 
{
    // ReadOnlySafe guarantees the linearizability of the read only request by
    // communicating with the quorum. It is the default and suggested option.
    ReadOnlySafe = 0,
    // ReadOnlyLeaseBased ensures linearizability of the read only request by
    // relying on the leader lease. It can be affected by clock drift.
    // If the clock drift is unbounded, leader might keep the lease longer than it
    // should (clock can move backward/pause without any bound). ReadIndex is not safe
    // in that case.
    ReadOnlyLeaseBased = 1
};


enum ErrorCode 
{
    OK                                = 0,
    ErrCompacted                      = 1,
    ErrSnapOutOfDate                  = 2,
    ErrUnavailable                    = 3,
    ErrSnapshotTemporarilyUnavailable = 4,
    ErrSeriaFail                      = 5
};

enum StateType 
{
    StateFollower = 0,
    StateCandidate = 1,
    StateLeader = 2,
    StatePreCandidate = 3,
    numStates
};

struct SoftState 
{
    uint64_t leader;
    StateType state;

    SoftState()
      : leader(None)
      , state(StateFollower) {}

    inline SoftState& operator=(const SoftState& from) 
    {
        leader = from.leader;
        state  = from.state;
        return *this;
    }
};

// ReadState provides state for read only query.
// It's caller's responsibility to call ReadIndex first before getting
// this state from ready, It's also caller's duty to differentiate if this
// state is what it requests through RequestCtx, eg. given a unique id as
// RequestCtx
struct ReadState 
{
    uint64_t index_;
    string   requestCtx_;
    ReadState(uint64_t index, const string &ctx)
      : index_(index),
        requestCtx_(ctx) {}
};

struct Ready 
{
 	// The current volatile state of a Node.
	// SoftState will be nil if there is no update.
	// It is not required to consume or store SoftState.
    SoftState         softState;

	// The current state of a Node to be saved to stable storage BEFORE
	// Messages are sent.
	// HardState will be equal to empty state if there is no update.
    HardState         hardState;

 	// ReadStates can be used for node to serve linearizable read requests locally
	// when its applied index is greater than the index in ReadState.
	// Note that the readState will be returned when raft receives msgReadIndex.
	// The returned is only valid for the request that requested to read.
    vector<ReadState*> readStates;

	// Entries specifies entries to be saved to stable storage BEFORE
	// Messages are sent.
    EntryVec          entries;

    // Snapshot specifies the snapshot to be saved to stable storage.
    Snapshot          *snapshot;

	// CommittedEntries specifies entries to be committed to a
	// store/state-machine. These have previously been committed to stable
	// store.
    EntryVec          committedEntries;

 	// Messages specifies outbound messages to be sent AFTER Entries are
	// committed to stable storage.
	// If it contains a MsgSnap message, the application MUST report back to raft
	// when the snapshot has been received or has failed by calling ReportSnapshot.
    vector<Message*>  messages;
};

bool isHardStateEqual(const HardState& h1, const HardState& h2);
bool isSoftStateEqual(const SoftState& s1, const SoftState& s2);
bool isEmptySnapshot(const Snapshot* snapshot);
int numOfPendingConf(const EntryVec& entries);

inline bool SUCCESS(int err) { return err == OK; }

bool isLoaclMessage(const MessageType msgt);
bool isResponseMessage(const MessageType msgt);
MessageType voteRespMsgType(int t);
void limitSize(EntryVec* ents, uint64_t maxSize);


const char* GetErrorString(int err);
string joinStrings(const vector<string>& strs, const string &sep);


#endif  // __UTIL_H__
