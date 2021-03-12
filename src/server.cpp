//
// Created by ctoast on 2021/3/11.
//

#include "server.h"
#include "utils.h"

int ServerObj::deployVMachine(int nodeIndex, VirtualMachineObj &obj) {
    auto info=obj.info;
    int memorySize=info.memorySize;
    int cpuNum=info.cpuNum;
    if(info.doubleNode){
        memorySize/=2;
        cpuNum/=2;
    }
    nodes[nodeIndex].remainingMemorySize-=memorySize;
    nodes[nodeIndex].remainingCPUNum-=cpuNum;

    //temporary place here;
    obj.deployedNodes.push_back(nodeIndex);
    obj.deployedServerID=ID;

    if(nodes[nodeIndex].remainingMemorySize<0||nodes[nodeIndex].remainingCPUNum<0){
        LOGE("server remaining resource less than zero");
        exit(-1);
    }

    return 0;
}

int ServerObj::delVMachine(int nodeIndex, VirtualMachineObj &obj) {
    auto info=obj.info;
    int memorySize=info.memorySize;
    int cpuNum=info.cpuNum;
    if(info.doubleNode){
        memorySize/=2;
        cpuNum/=2;
    }
    nodes[nodeIndex].remainingMemorySize+=memorySize;
    nodes[nodeIndex].remainingCPUNum+=cpuNum;

    return 0;
}
