#ifndef __NODE_H__
#define __NODE_H__

#include "raft.pb.h"
#include "util.h"

using namespace std;
using namespace raftpb;

struct Config;
struct Peer;

enum NodeMessageType 
{
    ProposeMessage    = 0,
    RecvMessage       = 1,
    ConfChangeMessage = 2,
    TickMessage       = 3,
    ReadyMessage      = 4,
    NoneMessage       = 6
};

class Node
{
public:
	// Tick increments the internal logical clock for the Node by a single tick. Election
	// timeouts and heartbeat timeouts are in units of ticks.
    virtual void Tick(Ready **ready) = 0;

    // Campaign causes the Node to transition to candidate state and start campaigning to become leader.
    virtual int Campaign(Ready **ready) = 0;

    // Propose proposes that data be appended to the log.
    virtual int Propose(const string& data, Ready **ready) = 0;

	// ProposeConfChange proposes config change.
	// At most one ConfChange can be in the process of going through consensus.
	// Application needs to call ApplyConfChange when applying EntryConfChange type entry.
    virtual int ProposeConfChange(const ConfChange& cc, Ready **ready) = 0;

    // Step advances the state machine using the given message. ctx.Err() will be returned, if any.
    virtual int Step(const Message& msg, Ready **ready) = 0;

 	// Advance notifies the Node that the application has saved progress up to the last Ready.
	// It prepares the node to return the next available Ready.
	//
	// The application should generally call Advance after it applies the entries in last Ready.
	//
	// However, as an optimization, the application may call Advance while it is applying the
	// commands. For example. when the last Ready contains a snapshot, the application might take
	// a long time to apply the snapshot data. To continue receiving Ready without blocking raft
	// progress, it can call Advance before finishing applying the last ready.
    virtual void Advance() = 0;

	// ApplyConfChange applies config change to the local node.
	// Returns an opaque ConfState protobuf which must be recorded
	// in snapshots. Will never return nil; it returns a pointer only
	// to match MemoryStorage.Compact.
    virtual void ApplyConfChange(const ConfChange& cc, ConfState *cs, Ready **ready) = 0;

    // TransferLeadership attempts to transfer leadership to the given transferee.
    virtual void TransferLeadership(uint64_t leader, uint64_t transferee, Ready **ready) = 0;

	// ReadIndex request a read state. The read state will be set in the ready.
	// Read state has a read index. Once the application advances further than the read
	// index, any linearizable read requests issued before the read request can be
	// processed safely. The read state will have the same rctx attached.
    virtual int ReadIndex(const string &rctx, Ready **ready) = 0;

	// Stop performs any necessary termination of the Node.
    virtual void Stop() = 0;
};

extern Node* StartNode(const Config *config, const vector<Peer>& peers);
extern Node* RestartNode(const Config *config);

#endif
