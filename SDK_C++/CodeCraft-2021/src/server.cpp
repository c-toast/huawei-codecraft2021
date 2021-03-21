//
// Created by ctoast on 2021/3/11.
//

#include "server.h"
#include "utils.h"

bool ServerInfo::canDeployOnSingleNode(VMInfo &vmInfo) {
    int doubleNode=vmInfo.doubleNode;
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
    int doubleNode=vmInfo.doubleNode;
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


int ServerObj::deployVM(int nodeIndex, VMObj* vmObj) {
    VMInfo vmInfo=vmObj->info;
    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);

    if(nodeIndex==NODEAB){
        nodes[NODEA].remainingResource.allocResource(requiredRes);
        nodes[NODEB].remainingResource.allocResource(requiredRes);

    }else {
        nodes[nodeIndex].remainingResource.allocResource(requiredRes);

    }
    vmObjMap.insert({vmObj->id, vmObj});

    return 0;
}

int ServerObj::delVM(int vmID) {
    Resource requiredRes;
    VMObj* vmObj=vmObjMap[vmID];
    for(auto nodeIndex:vmObj->deployNodes){
        vmObj->info.getRequiredResourceForOneNode(requiredRes);
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
    int doubleNode=vmInfo.doubleNode;
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
    int doubleNode=vmInfo.doubleNode;
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
    id=serverID;
    for(auto& it:vmObjMap){
        it.second->deployServerID=id;
    }
    return 0;
}



