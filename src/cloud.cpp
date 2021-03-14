//
// Created by ctoast on 2021/3/11.
//

#include "cloud.h"
#include "utils.h"

//the type should be Cloud*
SimpleCloud* globalCloud=new SimpleCloud();

int SimpleCloud::addServerObj(ServerInfo &serverInfo) {
    ServerObj obj(serverInfo,serverObjList.size());
    serverObjList.push_back(obj);
    return 0;
}

int SimpleCloud::addVMObj(int serverObjID, int nodeIndex, std::string vmModel, int vmID) {
    if(serverObjID < 0 || serverObjID > serverObjList.size() - 1){
        LOGE("serverObj id is wrong");
        exit(-1);
    }
    ServerObj& serverObj=serverObjList[serverObjID];
    VMInfo& vmInfo=vmInfoMap[vmModel];
    VMObj vmObj(vmInfo,vmID);

    int doubleNode;vmInfo.getDoubleNode(doubleNode);

    if(doubleNode==1){
        serverObj.deployVM(0, vmObj);
        serverObj.deployVM(1, vmObj);
    }else{
        serverObj.deployVM(nodeIndex,vmObj);
    }

    vmObjMap.insert({vmID, vmObj});
    return 0;
}

int SimpleCloud::delVMObj(int machineID) {
    auto machineIterator=vmObjMap.find(machineID);
    if(machineIterator == vmObjMap.end()){
        LOGE("can not find the vmObj in vmObj map");
        exit(-1);
    }
    auto vmObj=machineIterator->second;
    int serverID;vmObj.getDeployServerID(serverID);

    if(serverID<0||serverID>serverObjList.size()-1){
        LOGE("vmObj deployed id is wrong");
        exit(-1);
    }
    auto& server=serverObjList[serverID];

    vmObjMap.erase(machineIterator);

    VMInfo vmInfo;vmObj.getInfo(vmInfo);
    for(auto nodeIndex:vmObj.deployNodes){
        server.delVM(nodeIndex, vmInfo);
    }

    return 0;
}


int Cloud::getServerObjById(int id, ServerObj &receiver) {
    if(id<0||id>=serverObjList.size()){
        LOGE("getServerObjById: server id is invalid");
        exit(-1);
    }
    receiver= serverObjList[id];
    return 0;
}

int Cloud::getVMObjById(int id, VMObj &receiver) {
    auto it=vmObjMap.find(id);
    if(it==vmObjMap.end()){
        LOGE("getVMObjById: obj id is invalid");
        exit(-1);
    }
    receiver=it->second;
    return 0;
}

int Cloud::getVMInfoByModel(std::string model, VMInfo &receiver) {
    auto it=vmInfoMap.find(model);
    if(it==vmInfoMap.end()){
        LOGE("getVMInfoByModel: vm model is invalid");
        exit(-1);
    }
    receiver=it->second;
    return 0;
}
