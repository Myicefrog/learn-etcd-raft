#include "util.h"
#include "raft.pb.h"
#include <iostream>
#include <string>

using namespace std;
using namespace raftpb;

int main()
{
    bool ret = IsLocalMsg(MsgHup);
    cout<<"ret is "<<ret<<endl;
    return 0;
}
