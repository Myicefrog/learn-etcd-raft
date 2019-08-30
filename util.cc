#include <string>
#include <iostream>
#include "util.h"

bool IsLocalMsg(const MessageType msgt)
{
    return (msgt == MsgHup || msgt == MsgBeat || msgt == MsgUnreachable
        || msgt == MsgSnapStatus
        || msgt == MsgCheckQuorum);
}

bool IsResponseMsg(const MessageType msgt)
{
    return (msgt == MsgAppResp || msgt == MsgVoteResp || msgt == MsgHeartbeatResp  
        || msgt == MsgUnreachable    
        || msgt == MsgPreVoteResp);
}

MessageType voteRespMsgType(const MessageType msgt)
{
    switch (msgt)
    {
        case MsgVote:
            return MsgVoteResp;
        case MsgPreVote:
            return MsgPreVoteResp;
        default:
            cout<<"not a vote message"<<endl;
            return MsgPreVoteResp;
    }

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

