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

class CloudOperator{
public:
    int oldServerListSize=0;

    struct originDeployInfo{
        int originServerID;
        int originNodeIndex;
    };

    std::map<VMObj*,originDeployInfo> migrationMap;//mark the vm to be migrated
    std::deque<VMObj*> migrationVec;//record the order of migrated vm

    std::map<VMObj*,bool> newVMMap;

    std::map<VMObj*,ServerObj*> delVMMap;

    std::map<int,ServerObj> backupServerObj;


//    int deployVMObj(int serverObjID, int nodeIndex, int vmID);

    int deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj);

    int deployVMObjInFakeServerObj(ServerObj* serverObj,VMObj* vmObj,int nodeIndex);

    int markMigratedVMObj(ServerObj* serverObj, VMObj* vmObj);

    int delVMObjInFakeServerObj(ServerObj* serverObj, int vmID);

    int delVMObjFromCloud(int vmID);

    int genOneDayOpeRes(std::vector<Request> addReqVec, OneDayResult &receiver);

    ServerObj getFakeServerObj(ServerObj* serverObj);

    ServerObj getNewServerObj(ServerInfo serverInfo);

    int deployVMObjInNewServerObj(ServerObj* serverObj, VMObj* vmObj, int nodeIndex);

    int deployNewServerObj(ServerObj* ServerObj);

};


#endif //HUAWEI_CODECRAFT_CLOUDOPERATOR_H
