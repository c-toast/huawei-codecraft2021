//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_SERVER_H
#define HUAWEI_CODECRAFT_SERVER_H

#include "virtual-machine.h"

class ServerInfo{
public:
    char model[21];
    int cpuNum;
    int memorySize;
    int hardwareCost;
    int energyCost;
};

class ServerNode{
public:
    int remainingCPUNum;
    int remainingMemorySize;
};

class ServerObj{
public:
    ServerInfo info;
    int ID;
    ServerNode nodes[2];

    ServerObj()=delete;

    ServerObj(ServerInfo i){
        info=i;
        nodes[0].remainingCPUNum=i.cpuNum/2;
        nodes[1].remainingCPUNum=i.cpuNum/2;
        nodes[0].remainingMemorySize=i.memorySize/2;
        nodes[1].remainingMemorySize=i.memorySize/2;
    }

    int getNodeRemainingCpuNum(int nodeIndex){
        return nodes[nodeIndex].remainingCPUNum;
    };

    int getNodeRemainingMemorySize(int nodeIndex){
        return nodes[nodeIndex].remainingMemorySize;
    };

    int deployVMachine(int nodeIndex, VirtualMachineObj& obj);

    int delVMachine(int nodeIndex,VirtualMachineObj& obj);
};




#endif //HUAWEI_CODECRAFT_SERVER_H
