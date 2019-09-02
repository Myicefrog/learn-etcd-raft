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

enum ErrorCode {
  OK                                = 0,
  ErrCompacted                      = 1,
  ErrSnapOutOfDate                  = 2,
  ErrUnavailable                    = 3,
  ErrSnapshotTemporarilyUnavailable = 4,
  ErrSeriaFail                      = 5
};

inline bool SUCCESS(int err) { return err == OK; }

bool IsLocalMsg(const MessageType msgt);
bool IsResponseMsg(const MessageType msgt);
MessageType voteRespMsgType(const MessageType msgt);
void limitSize(EntryVec* ents, uint64_t maxSize);


const char* GetErrorString(int err);

#endif  // __UTIL_H__
