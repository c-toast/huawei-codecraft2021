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
    for(auto h:beforelistenerList){
        h->deployVMObj(serverObjID,nodeIndex,vmObj);
    }

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
    for(auto h:beforelistenerList){
        h->delVMObjFromCloud(vmID);
    }

    auto vmObj=vmObjMap[vmID];
    if(vmObj->deployServerID!=-1){
        moveVMObjFromServerObj(vmID);
    }

    usedRes[0]-=vmObj->info.cpuNum;
    usedRes[1]-=vmObj->info.memorySize;

    delete vmObj;
    vmObjMap.erase(vmID);

//    auto vmObj=vmObjMap[vmID];
//    int serverID=vmObj->deployServerID;
//    auto server=serverObjList[serverID];
//    server->delVM(vmID);

    return 0;
}

//int Cloud::eraseVMObj(int vmID) {
//    auto vmObj=vmObjMap[vmID];
//    delete vmObj;
//    vmObjMap.erase(vmID);
//    return 0;
//}


int Cloud::createServerObj(ServerInfo &serverInfo) {
    for(auto h:beforelistenerList){
        h->createServerObj(serverInfo);
    }
    auto* serverObj=new ServerObj(serverInfo);
    int id=serverObjList.size();
    serverObj->id=id;
    serverObjList.push_back(serverObj);

    modelServerMap[serverInfo.model].push_back(serverObj);

    ownRes[0]+=serverInfo.cpuNum;
    ownRes[1]+=serverInfo.memorySize;

    return id;
}

int Cloud::deployServerObj(ServerObj objTemplate) {
    for(auto h:beforelistenerList){
        h->deployServerObj(objTemplate);
    }

    int id=createServerObj(objTemplate.info);
    for(auto it:objTemplate.vmObjMap){
        deployVMObj(id,objTemplate.vmObjDeployNodeMap[it.first],it.second);
//        VMObj* vmObj=it.second;
//        int nodeIndex=serverObj->vmObjDeployNodeMap[it.first];
//        if(nodeIndex==NODEAB){
//            vmObj->deployNodes.push_back(NODEA);
//            vmObj->deployNodes.push_back(NODEB);
//        }else{
//            vmObj->deployNodes.push_back(nodeIndex);
//        }
//        vmObj->deployServerID=id;
    }
    return 0;
}

VMObj * Cloud::createVMObj(int vmID, std::string model) {
    for(auto h:beforelistenerList){
        h->createVMObj(vmID, model);
    }

    VMInfo info=vmInfoMap[model];
    auto vmObj=new VMObj(info,vmID);
    vmObjMap.insert({vmID,vmObj});

    usedRes[0]+=info.cpuNum;
    usedRes[1]+=info.memorySize;

    return vmObj;
}


int Cloud::moveVMObjFromServerObj(int vmID) {
    for(auto h:beforelistenerList){
        h->moveVMObjFromServerObj(vmID);
    }
//    if(vmID<0||vmID>serverObjList.size()){
//        LOGE("Cloud::moveVMObjFromServerObj: id is invalid");
//        exit(-1);
//    }

    auto vmObj=vmObjMap[vmID];
    int serverID=vmObj->deployServerID;
    if(serverID==-1){
        LOGE("break point");
        exit(-1);
    }
    auto server=serverObjList[serverID];

    server->delVM(vmID);
    vmObj->deployServerID=-1;
    vmObj->deployNodes.clear();
    return 0;
}




