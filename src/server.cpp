//
// Created by ctoast on 2021/3/11.
//

#include "server.h"
#include "utils.h"

int ServerInfo::getCpuNum(int& receiver){receiver=cpuNum;return 0;}

int ServerInfo::getMemorySize(int& receiver){receiver=memorySize;return 0;}

int ServerInfo::getModel(std::string &receiver) {receiver=model;return 0;}

int ServerInfo::getHardwareCost(int &receiver) {receiver=hardwareCost;return 0;}

int ServerInfo::getEnergyCost(int &receiver) {receiver=energyCost;return 0;}

bool ServerInfo::canDeployOnSingleNode(VMInfo &vmInfo) {
    int doubleNode;vmInfo.getDoubleNode(doubleNode);
    if(doubleNode == DOUBLEDEPLOY){
        return false;
    }

    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);
    Resource ownRes(cpuNum/2,memorySize/2);
    if(ownRes.memorySize<requiredRes.memorySize||ownRes.cpuNum<requiredRes.cpuNum){
        return false;
    }
    return true;
}

bool ServerInfo::canDeployOnDoubleNode(VMInfo &vmInfo) {
    int doubleNode;vmInfo.getDoubleNode(doubleNode);
    if(doubleNode == SINGLEDEPLOY){
        return false;
    }

    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);
    Resource ownRes(cpuNum/2,memorySize/2);
    if(ownRes.memorySize<requiredRes.memorySize||ownRes.cpuNum<requiredRes.cpuNum){
        return false;
    }
    return true;
}


int ServerObj::deployVM(int nodeIndex, VMObj &receiver) {
    VMInfo vmInfo;
    receiver.getInfo(vmInfo);
    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);
    nodes[nodeIndex].remainingResource.allocResource(requiredRes);
    receiver.deploy(ID,nodeIndex);//record the deploy message in VMOBJ

    return 0;
}

int ServerObj::delVM(int nodeIndex, VMInfo &vmInfo) {
    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);
    nodes[nodeIndex].remainingResource.freeResource(requiredRes);

    return 0;
}

int ServerObj::getNodeRemainingResource(int nodeIndex, Resource &receiver) {
    receiver = nodes[nodeIndex].remainingResource;
    return 0;
}

bool ServerObj::canDeployOnSingleNode(int nodeIndex, VMInfo &vmInfo) {
    int doubleNode;vmInfo.getDoubleNode(doubleNode);
    if(doubleNode == DOUBLEDEPLOY){
        return false;
    }

    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);
    Resource ownRes=nodes[nodeIndex].remainingResource;
    if(ownRes.memorySize<requiredRes.memorySize||ownRes.cpuNum<requiredRes.cpuNum){
        return false;
    }
    return true;
}

bool ServerObj::canDeployOnDoubleNode(VMInfo &vmInfo) {
    int doubleNode;vmInfo.getDoubleNode(doubleNode);
    if(doubleNode == SINGLEDEPLOY){
        return false;
    }

    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);
    Resource ownRes=nodes[0].remainingResource;
    if(ownRes.memorySize<requiredRes.memorySize||ownRes.cpuNum<requiredRes.cpuNum){
        return false;
    }
    ownRes=nodes[1].remainingResource;
    if(ownRes.memorySize<requiredRes.memorySize||ownRes.cpuNum<requiredRes.cpuNum){
        return false;
    }
    return true;
}



