#ifndef _RAFT_H__
#define _RAFT_H__

#include <map>
#include "util.h"
#include "log.h"
#include "progress.h"
#include "raft.pb.h"

using namespace std;
using namespace raftpb;

struct readOnly;
struct ReadState;

// Config contains the parameters to start a raft.
struct Config 
{
  // ID is the identity of the local raft. ID cannot be 0.
    uint64_t          id;

  // peers contains the IDs of all nodes (including self) in the raft cluster. It
  // should only be set when starting a new raft cluster. Restarting raft from
  // previous configuration will panic if peers is set. peer is private and only
  // used for testing right now.
    vector<uint64_t>  peers;

  // ElectionTick is the number of Node.Tick invocations that must pass between
  // elections. That is, if a follower does not receive any message from the
  // leader of current term before ElectionTick has elapsed, it will become
  // candidate and start an election. ElectionTick must be greater than
  // HeartbeatTick. We suggest ElectionTick = 10 * HeartbeatTick to avoid
  // unnecessary leader switching.
    int               electionTick;

  // HeartbeatTick is the number of Node.Tick invocations that must pass between
  // heartbeats. That is, a leader sends heartbeat messages to maintain its
  // leadership every HeartbeatTick ticks.
    int               heartbeatTick;

  // Storage is the storage for raft. raft generates entries and states to be
  // stored in storage. raft reads the persisted entries and states out of
  // Storage when it needs. raft reads out the previous state and configuration
  // out of storage when restarting.
    Storage*          storage;

  // Applied is the last applied index. It should only be set when restarting
  // raft. raft will not return entries to the application smaller or equal to
  // Applied. If Applied is unset when restarting, raft might return previous
  // applied entries. This is a very application dependent configuration.
    uint64_t          applied;

  // MaxSizePerMsg limits the max size of each append message. Smaller value
  // lowers the raft recovery cost(initial probing and message lost during normal
  // operation). On the other side, it might affect the throughput during normal
  // replication. Note: math.MaxUint64 for unlimited, 0 for at most one entry per
  // message.
    uint64_t          maxSizePerMsg;

  // MaxInflightMsgs limits the max number of in-flight append messages during
  // optimistic replication phase. The application transportation layer usually
  // has its own sending buffer over TCP/UDP. Setting MaxInflightMsgs to avoid
  // overflowing that sending buffer. TODO (xiangli): feedback to application to
  // limit the proposal rate?
    uint64_t          maxInflightMsgs;

  // CheckQuorum specifies if the leader should check quorum activity. Leader
  // steps down when quorum is not active for an electionTimeout.
    bool checkQuorum;

  // PreVote enables the Pre-Vote algorithm described in raft thesis section
  // 9.6. This prevents disruption when a node that has been partitioned away
  // rejoins the cluster.
    bool preVote;

  // Logger is the logger used for raft log. For multinode which can host
  // multiple raft group, each raft group can have its own logger
    Logger*           logger;            

  // ReadOnlyOption specifies how the read only request is processed.
  //
  // ReadOnlySafe guarantees the linearizability of the read only request by
  // communicating with the quorum. It is the default and suggested option.
  //
  // ReadOnlyLeaseBased ensures linearizability of the read only request by
  // relying on the leader lease. It can be affected by clock drift.
  // If the clock drift is unbounded, leader might keep the lease longer than it
  // should (clock can move backward/pause without any bound). ReadIndex is not safe
  // in that case.
    ReadOnlyOption    readOnlyOption;
};

struct Peer {
  uint64_t Id;
  string   Context;
};

enum CampaignType 
{
  // campaignPreElection represents the first phase of a normal election when
  // Config.PreVote is true.
    campaignPreElection = 1,
  // campaignElection represents a normal (time-based) election (the second phase
  // of the election when Config.PreVote is true).
    campaignElection = 2,
  // campaignTransfer represents the type of leader transfer
    campaignTransfer = 3
};

struct raft;

typedef void (*stepFun)(raft *, const Message&);

struct raft 
{
    uint64_t id_;
    uint64_t term_;
    uint64_t vote_;

    vector<ReadState*> readStates_;
    raftLog *raftLog_;
    int maxInfilght_;
    uint64_t maxMsgSize_;
    map<uint64_t, Progress*> prs_;
    StateType state_;
    map<uint64_t, bool> votes_;
    vector<Message*> msgs_;

    uint64_t leader_;
    // leadTransferee is id of the leader transfer target when its value is not zero.
    // Follow the procedure defined in raft thesis 3.10.
    uint64_t leadTransferee_;
    // New configuration is ignored if there exists unapplied configuration.
    bool pendingConf_;
    readOnly* readOnly_;

    // number of ticks since it reached last electionTimeout when it is leader
    // or candidate.
    // number of ticks since it reached last electionTimeout or received a
    // valid message from current leader when it is a follower.
    int electionElapsed_;

    // number of ticks since it reached last heartbeatTimeout.
    // only leader keeps heartbeatElapsed.
    int heartbeatElapsed_;
    int heartbeatTimeout_;
    int electionTimeout_;

    bool checkQuorum_;
    bool preVote_;

    // randomizedElectionTimeout is a random number between
    // [electiontimeout, 2 * electiontimeout - 1]. It gets reset
    // when raft changes its state to follower or candidate.
    int randomizedElectionTimeout_;

    Logger* logger_;

    stepFun stateStep;

    raft(const Config *, raftLog *);
    void tick();
    const char* getCampaignString(CampaignType t);
    void loadState(const HardState &hs);
    void nodes(vector<uint64_t> *nodes);
    bool hasLeader();
    void softState(SoftState *ss);
    void hardState(HardState *hs);
    int quorum();
    void send(Message *msg);

    void sendAppend(uint64_t to);
    void sendHeartbeat(uint64_t to, const string &ctx);
    void bcastAppend();
    void bcastHeartbeat();
    void bcastHeartbeatWithCtx(const string &ctx);
    void becomeFollower(uint64_t term, uint64_t leader);
    void becomeCandidate();
    void becomePreCandidate();
    void becomeLeader();
    void campaign(CampaignType t);
    bool maybeCommit();
    void reset(uint64_t term);
    void appendEntry(EntryVec* entries);
    void handleAppendEntries(const Message& msg);
    void handleHeartbeat(const Message& msg);
    void handleSnapshot(const Message& msg);
    void tickElection();
    void tickHeartbeat();
    int  poll(uint64_t id, MessageType t, bool v);
    int  step(const Message& msg);
    bool promotable();
    bool restore(const Snapshot& snapshot);
    void delProgress(uint64_t id);
    void addNode(uint64_t id);
    void removeNode(uint64_t id);
    bool pastElectionTimeout();
    void resetRandomizedElectionTimeout();
    void setProgress(uint64_t id, uint64_t match, uint64_t next);
    void abortLeaderTransfer();
    void proxyMessage(const Message& msg);
    void readMessages(vector<Message*> *);
    bool checkQuorumActive();
    void sendTimeoutNow(uint64_t to);
    void resetPendingConf();
};

extern raft* newRaft(const Config *);
string entryString(const Entry& entry);

void stepLeader(raft *r, const Message& msg);
void stepCandidate(raft* r, const Message& msg);
void stepFollower(raft* r, const Message& msg);

#endif
