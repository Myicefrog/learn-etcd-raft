#ifndef __NODE_IMP_H__
#define __NODE_IMP_H__

#include "node.h"
#include "raft.h"
#include "log.h"
#include "util.h"

using namespace std;
using namespace raftpb;

class NodeImpl : public Node
{
public:
    NodeImpl();
    ~NodeImpl();

    virtual void Tick(Ready **ready);
    virtual int  Campaign(Ready **ready);
    virtual int  Propose(const string& data, Ready **ready);
    virtual int  ProposeConfChange(const ConfChange& cc, Ready **ready);
    virtual int  Step(const Message& msg, Ready **ready);
    virtual void Advance();
    virtual void ApplyConfChange(const ConfChange& cc, ConfState *cs, Ready **ready);
    virtual void TransferLeadership(uint64_t leader, uint64_t transferee, Ready **ready);
    virtual int  ReadIndex(const string &rctx, Ready **ready);
    virtual void Stop();

private:
    int stateMachine(const Message& msg, Ready **ready);
    Ready* newReady();
    int doStep(const Message& msg, Ready **ready);
    bool isMessageFromClusterNode(const Message& msg);
    void handleConfChange();
    void handleAdvance();
    void reset();
    bool readyContainUpdate();

public:
    bool stopped_;
    raft *raft_;
    Logger *logger_;

    // save previous the state
    uint64_t leader_;
    SoftState prevSoftState_;
    HardState prevHardState_;
    bool waitAdvanced_;
    Ready ready_;
    bool canPropose_;
    NodeMessageType msgType_;

    uint64_t prevLastUnstableIndex_;
    uint64_t prevLastUnstableTerm_;
    bool     havePrevLastUnstableIndex_;
    uint64_t prevSnapshotIndex_;

    // for ApplyConfChange 
    ConfChange confChange_;
    ConfState*  confState_;
};

#endif
