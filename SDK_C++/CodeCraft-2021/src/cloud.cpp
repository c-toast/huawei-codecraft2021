//
// Created by ctoast on 2021/3/11.
//

#include "cloud.h"
#include "utils.h"

//the type should be Cloud*
SimpleCloud* globalCloud=new SimpleCloud();

int SimpleCloud::deployVMObj(int serverObjID, int nodeIndex, int vmID) {
    if(serverObjID < 0 || serverObjID > serverObjList.size() - 1){
        LOGE("serverObj id is wrong");
        exit(-1);
    }
    ServerObj* serverObj=serverObjList[serverObjID];
    VMObj* vmObj=vmObjMap[vmID];

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

int SimpleCloud::deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj) {
    if(serverObjID < 0 || serverObjID > serverObjList.size() - 1){
        LOGE("serverObj id is wrong");
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

int SimpleCloud::delVMObj(int vmID) {
    auto vmObj=vmObjMap[vmID];
    int serverID=vmObj->deployServerID;
    auto server=serverObjList[serverID];
    server->delVM(vmID);
    delete vmObj;
    vmObjMap.erase(vmID);
    return 0;
}

int SimpleCloud::createServerObj(ServerInfo &serverInfo) {
    auto* serverObj=new ServerObj(serverInfo);
    int id=serverObjList.size();
    serverObj->deployItselfInCloud(id);
    serverObjList.push_back(serverObj);
    return id;
}

VMObj * SimpleCloud::createVMObj(int vmID, std::string model) {
    VMInfo info=vmInfoMap[model];
    auto vmObj=new VMObj(info,vmID);
    vmObjMap.insert({vmID,vmObj});
    return vmObj;
}

int SimpleCloud::renewServerID(int start) {
    for(int i=start;i<serverObjList.size();i++){
        serverObjList[i]->id=i;
        for(auto& it:serverObjList[i]->vmObjMap){
            it.second->deployServerID=i;
        }
    }
    return 0;
}

int SimpleCloud::MigrateVMObj(int vmID) {
    auto vmObj=vmObjMap[vmID];
    int serverID=vmObj->deployServerID;
    auto server=serverObjList[serverID];
    server->delVM(vmID);
    vmObj->deployServerID=-1;
    vmObj->deployNodes.clear();
    return 0;
}

