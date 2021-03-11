//
// Created by ctoast on 2021/3/11.
//

#include "cloud.h"
#include "utils.h"

//the type should be Cloud*
SimpleCloud* globalCloud=new SimpleCloud();

int SimpleCloud::AddServerObj(ServerObj server) {
    int id=serverObjList.size();
    server.ID=id;
    serverObjList.push_back(server);
    return id;
}

int SimpleCloud::AddMachine(int serverId, VirtualMachineObj &machine) {
    if(serverId<0||serverId>serverObjList.size()-1){
        LOGE("server id is wrong");
        exit(-1);
    }
    ServerObj& server=serverObjList[serverId];
    VirtualMachineInfo info=machine.info;
    server.remainingCPUNum-=info.cpuNum;
    server.remainingMemorySize-=info.memorySize;
    if(server.remainingMemorySize<0||server.remainingCPUNum<0){
        LOGE("server remaining resource less than zero");
        exit(-1);
    }
    machine.deployedServerID=serverId;
    vMachineObjMap.insert({machine.ID,machine});
    return 0;
}

int SimpleCloud::DelMachine(int machineID) {
    auto machineIterator=vMachineObjMap.find(machineID);
    if(machineIterator==vMachineObjMap.end()){
        LOGE("can not find the machine in machine map");
        exit(-1);
    }
    auto machine=machineIterator->second;
    auto machineInfo=machine.info;
    int serverID=machine.deployedServerID;
    if(serverID<0||serverID>serverObjList.size()-1){
        LOGE("machine deployed id is wrong");
        exit(-1);
    }
    auto server=serverObjList[serverID];

    vMachineObjMap.erase(machineIterator);
    server.remainingCPUNum+=machineInfo.cpuNum;
    server.remainingMemorySize+=machineInfo.memorySize;

    return 0;
}

