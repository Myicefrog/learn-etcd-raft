#include <string>
#include <iostream>
#include "util.h"

bool isHardStateEqual(const HardState& h1, const HardState& h2) 
{
    return h1.term() == h2.term() &&
           h1.vote() == h2.vote() &&
           h1.commit() == h2.commit();
}

bool isSoftStateEqual(const SoftState& s1, const SoftState& s2) 
{
    if (s1.leader != s2.leader) 
    {
        return false;
    }

    return s1.state == s2.state;
}

bool isEmptySnapshot(const Snapshot* snapshot) 
{
    if (snapshot == NULL) 
    {
        return true;
    }
    return snapshot->metadata().index() == 0;
}

bool isLoaclMessage(const MessageType msgt)
{
    return (msgt == MsgHup || msgt == MsgBeat || msgt == MsgUnreachable
        || msgt == MsgSnapStatus
        || msgt == MsgCheckQuorum);
}

bool isResponseMessage(const MessageType msgt)
{
    return (msgt == MsgAppResp || msgt == MsgVoteResp || msgt == MsgHeartbeatResp  
        || msgt == MsgUnreachable    
        || msgt == MsgPreVoteResp);
}

MessageType voteRespMsgType(int t)
{
    if (t == MsgVote) 
    {
        return MsgVoteResp;
    }
    return MsgPreVoteResp;
}

void limitSize(EntryVec* ents, uint64_t maxSize)
{
    if(ents->size() == 0)
    {
        return;
    }

    uint64_t size = (*ents)[0].ByteSize();
    size_t limit;

    for(limit = 1; limit < ents->size(); limit++ )
    {
        size += (*ents)[limit].ByteSize();
        if(size > maxSize)
        {
            break;
        }
    
    }
    ents->erase(ents->begin() + limit, ents->end());

}

const char* GetErrorString(int err) 
{
    return "";
}

string joinStrings(const vector<string>& strs, const string &sep) {
  string ret = "";
  size_t i;
  for (i = 0; i < strs.size(); ++i) {
    if (ret.length() > 0) {
      ret += sep;
    }
    ret += strs[i];
  }

  return ret;
}

