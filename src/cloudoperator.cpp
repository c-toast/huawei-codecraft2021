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
    if(serverObjList.size() > oldServerListSize){
        std::sort(serverObjList.begin() + oldServerListSize, serverObjList.end(), objSetCmp);
    }
    //renew server id;
    for(int i=oldServerListSize; i < serverObjList.size(); i++){
        serverObjList[i]->id=i;
        for(auto& it:serverObjList[i]->vmObjMap){
            it.second->deployServerID=i;
            //renew delVMId
//            for(auto& delVMIt:serverDelVMMap){
//                if(delVMIt.second==serverObjList[i]){
//                    delVMIt.first->deployServerID=i;
//                }
//            }
        }
    }

    for(int i=oldServerListSize; i < serverObjList.size();){
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
        VMObj* obj=globalCloud->vmObjMap[it.vmID];
        d.serverID= obj->deployServerID;
        if(obj->info.doubleNode==1){
            d.node=NODEAB;
        }else{
            d.node=obj->deployNodes[0];
        }
        receiver.deployList.push_back(d);
    }

    migrationVec.clear();
    depTree.getMigrateVecInOrder(migrationVec);
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

    return 0;
}

int CloudOperator::deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj) {
    auto it=migrationMap.find(vmObj);

    if(it!=migrationMap.end()){
        ServerObj* serverObj=globalCloud->serverObjList[serverObjID];
        for(auto migrateIt:serverMigrateVMMap[serverObj]){
            if(depTree.isAncestor(migrateIt,vmObj)){
                return -1;
            }
        }
        for(auto migrateIt:serverMigrateVMMap[serverObj]){
            depTree.AddDep(vmObj,migrateIt);
        }
    }

    if(it!=migrationMap.end()){
        if(serverObjID==it->second.originServerID
           &&nodeIndex==it->second.originNodeIndex){
            migrationMap.erase(it);
        }
    }

    globalCloud->deployVMObj(serverObjID,nodeIndex,vmObj);
    return 0;
}

int CloudOperator::markMigratedVMObj(ServerObj *serverObj, VMObj *vmObj) {
    if(migrationMap.find(vmObj)!=migrationMap.end()){
        LOGE("CloudOperator::markMigratedVMObj: have already migrate the vmObj");
        exit(-1);
    }
    originDeployInfo i;
    i.originServerID=vmObj->deployServerID;
    i.originNodeIndex=vmObj->getDeployNode();

    migrationMap.insert({vmObj,i});
    migrationVec.push_back(vmObj);

    if(serverMigrateVMMap.find(serverObj)==serverMigrateVMMap.end()){
        serverMigrateVMMap.insert({serverObj,std::vector<VMObj*>()});
    }
    serverMigrateVMMap[serverObj].push_back(vmObj);

    globalCloud->MoveVMObjFromServerObj(vmObj->id);
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


//    auto it=serverObj->vmObjMap.find(vmObj->id);
//    if(it!=serverObj->vmObjMap.end()){
//        serverObj->vmObjMap.erase(it);
//        serverObj->vmObjDeployNodeMap.erase(vmObj->id);
//    }

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
//    for(auto mapIt:serverObj->vmObjMap){
//        VMObj* vmObj=mapIt.second;
//        auto it=migrationMap.find(vmObj);
//        if(it!=migrationMap.end()){
//            globalCloud->MoveVMObjFromServerObj(vmObj->id);
//        }
//    }

    globalCloud->deployServerObj(*serverObj);

    return 0;
}

int CloudOperator::getFakeServerObj(ServerObj *serverObj, ServerObj &receiver, int time) {
    if(serverObj!=globalCloud->serverObjList[serverObj->id]){
        LOGE("CloudOperator::getFakeServerObj: the serverObj do not match the server in cloud");
        exit(-1);
    }
    receiver=*serverObj;
    std::vector<VMObj*>& delVMVec=serverDelVMMap[serverObj];//may have problem??
    for(auto it:delVMVec){
        int reqTime=vmReqTimeMap[it];
        if(reqTime>=time){//meanning the vm is delete after the time
            originDeployInfo i=delVMOriginInfoMap[it];
            if(!receiver.canDeployOnNode(i.originNodeIndex,it->info)){
                return -1;
            }
            receiver.deployVM(i.originNodeIndex,it);
        }
    }
    return 0;
}

ServerObj CloudOperator::getNewServerObj(ServerInfo serverInfo) {
    return ServerObj(serverInfo);
}

int CloudOperator::delVMObjFromCloud(int vmID) {
    globalCloud->delVMObjFromCloud(vmID);
    return 0;
}

int CloudOperator::initWhenNewDayStart(OneDayRequest &oneDayReq) {
    oldServerListSize=globalCloud->serverObjList.size();
    migrationMap.clear();
    migrationVec.clear();
    serverDelVMMap.clear();

    vmReqTimeMap.clear();
    serverMigrateVMMap.clear();
    delVMOriginInfoMap.clear();
    depTree.clear();

    for(auto it:oneDayReq){
        VMObj* vmObj;
        if(it.op==ADD) {
            vmObj= globalCloud->createVMObj(it.vmID, it.vMachineModel);
            vmReqTimeMap[vmObj]=it.reqTime;
        }else{
            vmObj= globalCloud->vmObjMap[it.vmID];
            vmReqTimeMap[vmObj]=it.reqTime;

            originDeployInfo i;
            i.originServerID=vmObj->deployServerID;
            i.originNodeIndex=vmObj->getDeployNode();
            delVMOriginInfoMap[vmObj]=i;

            serverDelVMMap[globalCloud->serverObjList[i.originServerID]].push_back(vmObj);

            globalCloud->MoveVMObjFromServerObj(it.vmID);
        }
    }
    return 0;
}


int MigrateDependencyTree::init(std::vector<VMObj *> migrateVMVec) {
    for(auto it:migrateVMVec){
        migrateTime[it]=-1;
        Node* n =new Node();
        n->vmObj=it;
        vmObjNodeMap[it]=n;
        migrateTime[it]=-1;
    }
    return 0;
}

//avoid self dep!!!
int MigrateDependencyTree::AddDep(VMObj *dependent, VMObj *target) {
    if(isAncestor(target,dependent)||dependent==target){
        return -1;
    }
    Node* depNode=vmObjNodeMap[dependent];
    Node* tarNode=vmObjNodeMap[target];
    depNode->children.push_back(tarNode);
    tarNode->parents.push_back(depNode);

    if(migrateTime[target]>=migrateTime[dependent]){
        migrateTime[target]=migrateTime[dependent]-1;
        renewMigrateTime(target);
    }

    return 0;
}

int MigrateDependencyTree::clear() {
    for(auto it:vmObjNodeMap){
        delete it.second;
    }
    migrateTime.clear();
    vmObjNodeMap.clear();
    return 0;
}

int MigrateDependencyTree::renewMigrateTime(VMObj *root) {
    for(auto it:vmObjNodeMap[root]->children){
        if(migrateTime[it->vmObj]>=migrateTime[root]){
            migrateTime[it->vmObj]=migrateTime[root]-1;
            renewMigrateTime(it->vmObj);
        }
    }
    return 0;
}

bool MigrateDependencyTree::isAncestor(VMObj *parent, VMObj *child) {

    Node* childNode=vmObjNodeMap[child];
    for(auto it:childNode->parents){
        if(it->vmObj==parent){
            return true;
        }
        if(isAncestor(parent,it->vmObj)){
            return true;
        }
    }
    return false;
}

int MigrateDependencyTree::getMigrateVecInOrder(std::vector<VMObj *> &receiver) {
    auto& mt=migrateTime;
    auto Cmp = [mt](VMObj* v1, VMObj* v2) {
        auto it1=mt.find(v1);
        auto it2=mt.find(v2);
        return it1->second<it2->second;
    };

    for(auto it:migrateTime){
        receiver.push_back(it.first);
    }
    std::sort(receiver.begin(),receiver.end(),Cmp);
    return 0;
}
