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
    if(Resource::isResourceEnough(ownRes,requiredRes)){
        return true;
    }
    return false;
}

bool ServerInfo::canDeployOnDoubleNode(VMInfo &vmInfo) {
    int doubleNode=vmInfo.doubleNode;
    if(doubleNode == SINGLEDEPLOY){
        return false;
    }

    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);
    Resource ownRes(cpuNum/2,memorySize/2);
    if(!Resource::isResourceEnough(ownRes,requiredRes)){
        return false;
    }
    return true;
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
    if(!Resource::isResourceEnough(ownRes,requiredRes)){
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
    if(!Resource::isResourceEnough(ownRes,requiredRes)){
        return false;
    }
    ownRes=nodes[1].remainingResource;
    if(!Resource::isResourceEnough(ownRes,requiredRes)){
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
        if(Resource::CalResourceMagnitude(resA)>Resource::CalResourceMagnitude(resB)){
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

//int ServerObj::deployItselfInCloud(int serverID) {
//    id=serverID;
//    for(auto& it:vmObjMap){
//        it.second->deployServerID=id;
//    }
//    return 0;
//}

bool ServerObj::canDeployOnNode(int nodeIndex, VMInfo &vmInfo) {
    if(nodeIndex==NODEAB){
        return canDeployOnDoubleNode(vmInfo);
    }else{
        return canDeployOnSingleNode(nodeIndex,vmInfo);
    }
}

int ServerObj::deployVM(int nodeIndex, VMObj* vmObj) {
    allocResForDeploy(nodeIndex,vmObj);

    int vmID=vmObj->id;
    if(vmObjMap.find(vmID)!=vmObjMap.end()||vmObjDeployNodeMap.find(vmID)!=vmObjDeployNodeMap.end()){
        LOGE("ServerObj::deployVM: the vmObj have already been deploy in this serverObj");
        exit(-1);
    }
    vmObjMap.insert({vmID, vmObj});
    vmObjDeployNodeMap.insert({vmID,nodeIndex});

    return 0;
}

int ServerObj::delVM(int vmID) {
    VMObj* vmObj=vmObjMap[vmID];
    if(vmObj==NULL){
        LOGE("ServerObj::delVM: the vm does not exist in this obj");
        return -1;
    }
    int nodeIndex=vmObj->info.doubleNode==1?NODEAB:vmObj->deployNodes[0];
    delVM(nodeIndex,vmObj);

    return 0;
}

int ServerObj::delVM(int nodeIndex, VMObj *vmObj) {
    int vmID=vmObj->id;
    freeResForDel(nodeIndex,vmObj);
    vmObjMap.erase(vmID);
    vmObjDeployNodeMap.erase(vmID);

    return 0;
}

int ServerObj::allocResForDeploy(int nodeIndex, VMObj *vmObj) {
    VMInfo vmInfo=vmObj->info;
    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);

    if(nodeIndex==NODEAB){
        nodes[NODEA].remainingResource.allocResource(requiredRes);
        nodes[NODEB].remainingResource.allocResource(requiredRes);
    }else {
        nodes[nodeIndex].remainingResource.allocResource(requiredRes);
    }
    return 0;
}

int ServerObj::freeResForDel(int nodeIndex, VMObj *vmObj) {
    Resource requiredRes;
    vmObj->info.getRequiredResourceForOneNode(requiredRes);
    if(nodeIndex==NODEAB){
        nodes[NODEA].remainingResource.freeResource(requiredRes);
        nodes[NODEB].remainingResource.freeResource(requiredRes);
    }else {
        nodes[nodeIndex].remainingResource.freeResource(requiredRes);
    }
    return 0;
}





