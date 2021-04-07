//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_CLOUDOPERATOR_H
#define HUAWEI_CODECRAFT_CLOUDOPERATOR_H

#include "vm.h"
#include "server.h"
#include "map"
#include "readwriter.h"
#include <deque>

class MigrateDependencyTree{
public:
    struct Node{
        VMObj* vmObj;
        std::vector<Node*> parents;
        std::vector<Node*> children;
    };

    std::map<VMObj*, int> migrateTime;
    std::map<VMObj*, Node*> vmObjNodeMap;

    int init(std::vector<VMObj*> migrateVMVec);

    int clear();

    int AddDep(VMObj* dependent,VMObj* target);

    int renewMigrateTime(VMObj* root);

    bool isAncestor(VMObj* parent, VMObj* child);

    int getMigrateVecInOrder(std::vector<VMObj*>& receiver);
};

class CloudOperator{
public:
    int oldServerListSize=0;

    struct originDeployInfo{
        int originServerID;
        int originNodeIndex;
    };

    std::map<VMObj*,originDeployInfo> migrationMap;//mark the vm to be migrated
    std::vector<VMObj*> migrationVec;//record the order of migrated vm

//    std::map<VMObj*,bool> newVMMap;//used to stop migrate new VM

//    //used to renew delVM migrate server id. useless because we do not migrate del VM now
//    std::map<VMObj*,ServerObj*> serverDelVMMap;


    std::map<VMObj*,int> vmReqTimeMap;

    std::map<ServerObj*,std::vector<VMObj*>> serverDelVMMap;

    std::map<ServerObj*,std::vector<VMObj*>> serverMigrateVMMap;

    std::map<VMObj*,originDeployInfo> delVMOriginInfoMap;//mark the vm to be del

    MigrateDependencyTree depTree;

    int initWhenNewDayStart(OneDayRequest &oneDayReq);

    int deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj);

    int getFakeServerObj(ServerObj *serverObj, ServerObj &receiver, int time);

    int deployVMObjInFakeServerObj(ServerObj* serverObj,VMObj* vmObj,int nodeIndex);

    int delVMObjInFakeServerObj(ServerObj* serverObj, int vmID);

    int markMigratedVMObj(ServerObj* serverObj, VMObj* vmObj);

    int delVMObjFromCloud(int vmID);

    int genOneDayOpeRes(std::vector<Request> addReqVec, OneDayResult &receiver);

    ServerObj getNewServerObj(ServerInfo serverInfo);

    int deployVMObjInNewServerObj(ServerObj* serverObj, VMObj* vmObj, int nodeIndex);

    int deployNewServerObj(ServerObj* ServerObj);
};




#endif //HUAWEI_CODECRAFT_CLOUDOPERATOR_H
