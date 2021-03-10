//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_CLOUD_H
#define HUAWEI_CODECRAFT_CLOUD_H

#include <vector>

#include "server.h"
#include "virtual-machine.h"

class Cloud{
    //addServerObj return server id
    virtual int AddServerObj(ServerObj server)=0;

    virtual int AddMachine(int serverId,VirtualMachine& machine)=0;

    virtual int DelMachine(VirtualMachine& machine)=0;

    virtual int MovMachine(int from,int to,VirtualMachine& machine)=0;

    virtual vector<int> FindServersByCondition()=0;
};


#endif //HUAWEI_CODECRAFT_CLOUD_H
