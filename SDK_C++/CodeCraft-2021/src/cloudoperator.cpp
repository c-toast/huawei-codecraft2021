//
// Created by ctoast on 2021/3/24.
//

#include "cloudoperator.h"
#include "algorithm"
#include "cloud.h"
#include "strategytools.h"

int cloudoperator::ouputOneDayRes(std::vector<Request> addReqVec, OneDayResult &receiver) {
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

//    for(auto& it:migrationMap){
//        Migration m;
//        m.virtualID=it.first->id;
//        m.serverID=it.first->deployServerID;
//        if(it.first->info.doubleNode==1){
//            m.node=NODEAB;
//        }else{
//            m.node=it.first->deployNodes[0];
//        }
//        receiver.migrationList.push_back(m);
//    }

    oldSize=serverObjList.size();
    migrationMap.clear();
    migrationVec.clear();

    return 0;
}

int cloudoperator::deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj) {
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

int cloudoperator::migrateVMObj(ServerObj *serverObj, VMObj *vmObj) {
    originDeployInfo i;
    i.originServerID=vmObj->deployServerID;
    if(vmObj->info.doubleNode==1){
        i.originNodeIndex=NODEAB;
    }else{
        i.originNodeIndex=vmObj->deployNodes[0];
    }
    //globalCloud->delVMObjFromServerObj(vmObj->id);
    migrationMap.insert({vmObj,i});

    return 0;
}

int cloudoperator::deployVMObjInServerObj(ServerObj *serverObj, VMObj *vmObj, int nodeIndex) {
    auto it=migrationMap.find(vmObj);
    if(it!=migrationMap.end()){
        globalCloud->delVMObjFromServerObj(vmObj->id);
        migrationVec.push_back(vmObj);
    }
    serverObj->deployVM(nodeIndex,vmObj);
    return 0;
}

