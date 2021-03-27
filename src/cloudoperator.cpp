//
// Created by ctoast on 2021/3/24.
//

#include "cloudoperator.h"
#include "algorithm"
#include "cloud.h"
#include "strategytools.h"
#include "global.h"

int CloudOperator::genOneDayOpeRes(std::vector<Request>addReqVec, OneDayResult &receiver) {
    auto objSetCmp=[](const ServerObj* s1,const ServerObj* s2){
        return s1->info.model<s2->info.model;
    };
    auto& serverObjList=globalCloud->serverObjList;
    if(serverObjList.size()>oldSize){
        std::sort(serverObjList.begin()+oldSize, serverObjList.end(), objSetCmp);
    }

    globalCloud->renewServerID(oldSize);

    for(int i=oldSize; i < serverObjList.size();){
        std::string model=serverObjList[i]->info.model;
        Purchase p;
        p.serverName=model;
        p.num=1;
        int j=i+1;
        for(;j<serverObjList.size();j++){
            if(serverObjList[j]->info.model==model){
                p.num++;
            }else{
                break;
            }
        }
        i=j;
        receiver.purchaseVec.push_back(p);
    }

    for(auto& it:addReqVec){
        Deploy d;
        VMObj* obj=globalCloud->vmObjMap[it.vMachineID];
        d.serverID= obj->deployServerID;
        if(obj->info.doubleNode==1){
            d.node=NODEAB;
        }else{
            d.node=obj->deployNodes[0];
        }
        receiver.deployList.push_back(d);
    }

    for(auto& it:migrationVec){
        auto map_it=migrationMap.find(it);
        if(map_it==migrationMap.end()){
            continue;
        }
        Migration m;
        m.virtualID=map_it->first->id;
        m.serverID=map_it->first->deployServerID;
        if(map_it->first->info.doubleNode==1){
            m.node=NODEAB;
        }else{
            m.node=map_it->first->deployNodes[0];
        }
        receiver.migrationList.push_back(m);
    }

    oldSize=serverObjList.size();
    migrationMap.clear();
    migrationVec.clear();

    return 0;
}

int CloudOperator::deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj) {
    auto it=migrationMap.find(vmObj);
    if(it!=migrationMap.end()){
        if(serverObjID==it->second.originServerID
           &&nodeIndex==it->second.originNodeIndex){
            migrationMap.erase(it);
        }

        globalCloud->delVMObjFromServerObj(vmObj->id);
        migrationVec.push_back(vmObj);
    }

    globalCloud->deployVMObj(serverObjID,nodeIndex,vmObj);
    return 0;
}

int CloudOperator::markMigratedVMObj(ServerObj *serverObj, VMObj *vmObj) {
    originDeployInfo i;
    i.originServerID=vmObj->deployServerID;
    if(vmObj->info.doubleNode==1){
        i.originNodeIndex=NODEAB;
    }else{
        i.originNodeIndex=vmObj->deployNodes[0];
    }
    migrationMap.insert({vmObj,i});

    return 0;
}

int CloudOperator::deployVMObjInNewServerObj(ServerObj *serverObj, VMObj *vmObj, int nodeIndex) {
    int serverID=serverObj->id;
    if(serverID!=-1){
        LOGE("CloudOperator::deployVMObjInNewServerObj: deploy on a old serverObj!");
        return -1;
    }
    serverObj->deployVM(nodeIndex,vmObj);
    return 0;
}

int CloudOperator::deployVMObjInFakeServerObj(ServerObj *serverObj, VMObj *vmObj, int nodeIndex) {
    int serverID=serverObj->id;
    if(serverID==-1||globalCloud->serverObjList[serverID]==serverObj){
        LOGE("CloudOperator::deployVMObjInFakeServerObj: deploy on a new or real serverObj!");
        return -1;
    }
    auto it=serverObj->vmObjMap.find(vmObj->id);
    if(it!=serverObj->vmObjMap.end()){
        serverObj->vmObjMap.erase(it);
        serverObj->vmObjDeployNodeMap.erase(vmObj->id);
    }

    serverObj->deployVM(nodeIndex,vmObj);

    return 0;
}

int CloudOperator::delVMObjInFakeServerObj(ServerObj *serverObj, int vmID) {
    int serverID=serverObj->id;
    if(serverID==-1||globalCloud->serverObjList[serverID]==serverObj){
        LOGE("CloudOperator::deployVMObjInFakeServerObj: deploy on a new or real serverObj!");
        return -1;
    }
    serverObj->delVM(vmID);

    return 0;
}

int CloudOperator::deployNewServerObj(ServerObj* serverObj) {
    int serverID=serverObj->id;
    if(serverID!=-1){
        LOGE("CloudOperator::deployNewServerObj: deploy a old serverObj!");
        return -1;
    }
    for(auto mapIt:serverObj->vmObjMap){
        VMObj* vmObj=mapIt.second;
        auto it=migrationMap.find(vmObj);
        if(it!=migrationMap.end()){
            globalCloud->delVMObjFromServerObj(vmObj->id);
            migrationVec.push_back(vmObj);
        }
    }

    globalCloud->deployServerObj(*serverObj);

    return 0;
}

ServerObj CloudOperator::getFakeServerObj(ServerObj *serverObj) {
    return ServerObj(*serverObj);
}

ServerObj CloudOperator::getNewServerObj(ServerInfo serverInfo) {
    return ServerObj(serverInfo);
}



