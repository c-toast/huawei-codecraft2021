//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_CLOUDOPERATOR_H
#define HUAWEI_CODECRAFT_CLOUDOPERATOR_H

#include "vm.h"
#include "server.h"
#include "map"
#include "readwriter.h"

class cloudoperator{
public:
    int oldSize=0;


    struct originDeployInfo{
        int originServerID;
        int originNodeIndex;
    };

    std::map<VMObj*,originDeployInfo> migrationMap;
    std::vector<VMObj*> migrationVec;

    int movVMObj();

    int deployVMObj(int serverObjID, int nodeIndex, int vmID);

    int deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj);

    int BuyServer();

    int ouputOneDayRes(std::vector<Request>,OneDayResult &receiver);

    int migrateVMObj(ServerObj* serverObj,VMObj* vmObj);

    int deployVMObjInServerObj(ServerObj* serverObj,VMObj* vmObj,int nodeIndex);
};


#endif //HUAWEI_CODECRAFT_CLOUDOPERATOR_H
