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

int SimpleCloud::AddMachine(int serverId, int nodeIndex, VirtualMachineObj &machineObj) {
    if(serverId<0||serverId>serverObjList.size()-1){
        LOGE("server id is wrong");
        exit(-1);
    }
    ServerObj& server=serverObjList[serverId];
    VirtualMachineInfo info=machineObj.info;

    if(info.doubleNode){
        server.deployVMachine(0, machineObj);
        server.deployVMachine(1, machineObj);
    }else{
        server.deployVMachine(nodeIndex,machineObj);
    }

    vMachineObjMap.insert({machineObj.ID, machineObj});
    return 0;
}

int SimpleCloud::DelMachine(int machineID) {
    auto machineIterator=vMachineObjMap.find(machineID);
    if(machineIterator==vMachineObjMap.end()){
        LOGE("can not find the machine in machine map");
        exit(-1);
    }
    auto machine=machineIterator->second;
    int serverID=machine.deployedServerID;

    if(serverID<0||serverID>serverObjList.size()-1){
        LOGE("machine deployed id is wrong");
        exit(-1);
    }
    auto& server=serverObjList[serverID];

    vMachineObjMap.erase(machineIterator);
    for(auto nodeIndex:machine.deployedNodes){
        server.delVMachine(nodeIndex,machine);
    }

    return 0;
}

