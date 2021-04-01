//
// Created by ctoast on 2021/3/11.
//

#include "cloud.h"

#include <utility>
#include "utils.h"

//int Cloud::deployVMObj(int serverObjID, int nodeIndex, int vmID) {
//    if(serverObjID < 0 || serverObjID > serverObjList.size() - 1){
//        LOGE("Cloud::deployVMObj: serverObj id is wrong");
//        exit(-1);
//    }
//    ServerObj* serverObj=serverObjList[serverObjID];
//    VMObj* vmObj=vmObjMap[vmID];
//
//    int doubleNode=vmObj->info.doubleNode;
//
//    if(doubleNode==1){
//        serverObj->deployVM(NODEAB, vmObj);
//        vmObj->deployNodes.push_back(NODEA);
//        vmObj->deployNodes.push_back(NODEB);
//    }else{
//        serverObj->deployVM(nodeIndex,vmObj);
//        vmObj->deployNodes.push_back(nodeIndex);
//    }
//    vmObj->deployServerID=serverObjID;
//
//    return 0;
//}

int Cloud::deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj) {
    if(serverObjID < 0 || serverObjID > serverObjList.size() - 1){
        LOGE("Cloud::deployVMObj: serverObj id is invalid");
        exit(-1);
    }
    ServerObj* serverObj=serverObjList[serverObjID];
    int doubleNode=vmObj->info.doubleNode;

    if(doubleNode==1){
        serverObj->deployVM(NODEAB, vmObj);
        vmObj->deployNodes.push_back(NODEA);
        vmObj->deployNodes.push_back(NODEB);
    }else{
        serverObj->deployVM(nodeIndex,vmObj);
        vmObj->deployNodes.push_back(nodeIndex);
    }
    vmObj->deployServerID=serverObjID;

    return 0;
}

int Cloud::delVMObjFromCloud(int vmID) {
    auto vmObj=vmObjMap[vmID];
    int serverID=vmObj->deployServerID;
    auto server=serverObjList[serverID];
    if(server==NULL){
        LOGE("delVMObjFromCloud: can not find the serverObj in cloud");
        exit(-1);
    }
    server->delVM(vmID);

    delete vmObj;
    vmObjMap.erase(vmID);
    return 0;
}

//int Cloud::createServerObj(ServerInfo &serverInfo) {
//    auto* serverObj=new ServerObj(serverInfo);
//    int id=serverObjList.size();
//    serverObj->deployItselfInCloud(id);
//    serverObjList.push_back(serverObj);
//    return id;
//}

int Cloud::deployServerObj(ServerObj objTemplate) {
    auto* serverObj=new ServerObj(std::move(objTemplate));
    int id=serverObjList.size();
    serverObj->deployItselfInCloud(id);
    serverObjList.push_back(serverObj);

    for(auto it:serverObj->vmObjMap){
        VMObj* vmObj=it.second;
        int nodeIndex=serverObj->vmObjDeployNodeMap[it.first];
        if(nodeIndex==NODEAB){
            vmObj->deployNodes.push_back(NODEA);
            vmObj->deployNodes.push_back(NODEB);
        }else{
            vmObj->deployNodes.push_back(nodeIndex);
        }
        vmObj->deployServerID=id;
    }
    return 0;
}

VMObj * Cloud::createVMObj(int vmID, std::string model) {
    VMInfo info=vmInfoMap[model];
    auto vmObj=new VMObj(info,vmID);
    vmObjMap.insert({vmID,vmObj});

    return vmObj;
}


int Cloud::delVMObjFromServerObj(int vmID) {
    auto vmObj=vmObjMap[vmID];
    int serverID=vmObj->deployServerID;
    auto server=serverObjList[serverID];

    server->delVM(vmID);
    vmObj->deployServerID=-1;
    vmObj->deployNodes.clear();
    return 0;
}



