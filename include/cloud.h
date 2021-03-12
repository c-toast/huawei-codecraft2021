//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_CLOUD_H
#define HUAWEI_CODECRAFT_CLOUD_H

#include <vector>
#include <map>

#include "server.h"
#include "virtual-machine.h"
#include "server.h"
#include "virtual-machine.h"

class Cloud{
    //addServerObj return server id
    virtual int AddServerObj(ServerObj server)=0;

    virtual int AddMachine(int serverId, int nodeIndex, VirtualMachineObj &machineObj) =0;

    virtual int DelMachine(int machineID)=0;


    //virtual int MovMachine(int from,int to,VirtualMachine& machine)=0;

    //virtual vector<int> FindServersByCondition()=0;
};

class SimpleCloud: public Cloud{
public:
    SimpleCloud()=default;

    std::vector<ServerInfo> serverInfoList;

    std::map<std::string,VirtualMachineInfo> vMachineInfoMap; //[model]info

    std::vector<ServerObj> serverObjList;

    std::map<int,VirtualMachineObj> vMachineObjMap;//[id]obj
    //addServerObj return server id
    int AddServerObj(ServerObj server) override;

    int AddMachine(int serverId, int nodeIndex, VirtualMachineObj &machineObj) override;

    int DelMachine(int machineID) override;

};

#endif //HUAWEI_CODECRAFT_CLOUD_H
