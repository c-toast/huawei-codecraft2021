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
    VMInfo vmInfo=receiver.info;
    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);
    nodes[nodeIndex].remainingResource.allocResource(requiredRes);

    receiver.deployInCloud(ID);
    receiver.deployInServer(nodeIndex);

    if(vmObjMap.find(receiver.id)!=vmObjMap.end()){
        vmObjMap.erase(receiver.id);
    }
    vmObjMap.insert({receiver.id,receiver});

    return 0;
}

int ServerObj::delVM(int vmID) {
    Resource requiredRes;
    VMObj vmObj=vmObjMap[vmID];
    for(auto nodeIndex:vmObj.deployNodes){
        vmObj.info.getRequiredResourceForOneNode(requiredRes);
        nodes[nodeIndex].remainingResource.freeResource(requiredRes);
        vmObjMap.erase(vmID);
    }
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

bool ServerObj::canDeploy(VMInfo &vmInfo, int &deployNode) {
    if(canDeployOnDoubleNode(vmInfo)){
        deployNode=NODEAB;
        return true;
    }
    bool canDeployA=canDeployOnSingleNode(NODEA,vmInfo);
    bool canDeployB=canDeployOnSingleNode(NODEB,vmInfo);
    if(canDeployA&&canDeployB){
        Resource resA,resB;
        getNodeRemainingResource(NODEA,resA);
        getNodeRemainingResource(NODEA,resB);
        //may have problem here
        if(resA.memorySize>resB.memorySize&&resA.cpuNum>resB.cpuNum){
            deployNode=NODEA;
        }else{
            deployNode=NODEB;
        }
        return true;
    }else if(canDeployA){
        deployNode=NODEA;
        return true;
    }else if(canDeployB){
        deployNode=NODEB;
        return true;
    }

    return false;
}

int ServerObj::deployItselfInCloud(int serverID) {
    ID=serverID;
    for(auto& it:vmObjMap){
        it.second.deployServerID=ID;
    }
    return 0;
}



