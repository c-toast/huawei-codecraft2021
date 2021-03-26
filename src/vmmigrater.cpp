//
// Created by ctoast on 2021/3/24.
//

#include "vmmigrater.h"
#include "global.h"

#define MIGRATER_USAGESTATE_r0 0.3

int VMMigrater::migrate(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<ServerObj*> serverObjList=globalCloud->serverObjList;

    for(auto serverIt:globalCloud->serverObjList){
        if(availableMigrateTime == 0){
            return 0;
        }
        ServerObj tmpserverIt=cloudOperator.getFakeServerObj(serverIt);
        migrateByUsageState(unhandledVMObj,&tmpserverIt);
        migrateByNodeBalance(unhandledVMObj,&tmpserverIt);
    }
    return 0;
}

int VMMigrater::migrateByUsageState(std::vector<VMObj *> &unhandledVMObj, ServerObj *simulatedServerObj) {
    if (availableMigrateTime == 0) {
        return 0;
    }
    for (; !UsageState::isServerNodeInASD(simulatedServerObj,NODEAB, USAGESTATE_RO,MIGRATER_USAGESTATE_r0);) {
        int preMigrateTime=availableMigrateTime;
        for (auto vmMapIt:simulatedServerObj->vmObjMap) {
            std::string vmModel = vmMapIt.second->info.model;
            int range = fitnessMap[vmModel][simulatedServerObj->info.model];
            if (range > ACCEPT_RANGE) {
                cloudOperator.markMigratedVMObj(simulatedServerObj, vmMapIt.second);
                cloudOperator.delVMObjInFakeServerObj(simulatedServerObj, vmMapIt.second->id);
                unhandledVMObj.push_back(vmMapIt.second);
                availableMigrateTime--;
                break;
            }
        }
        if (availableMigrateTime == 0) {
            return 0;
        }
        if(preMigrateTime == availableMigrateTime){
            return 0;
        }
    }
}

int VMMigrater::migrateByNodeBalance(std::vector<VMObj *> &unhandledVMObj, ServerObj *simulatedServerObj) {
    if (availableMigrateTime == 0) {
        return 0;
    }
    for (; !UsageState::isServerNodeInASD(simulatedServerObj,NODEAB, USAGESTATE_RO,MIGRATER_USAGESTATE_r0);) {
        int preMigrateTime=availableMigrateTime;
        Resource rA, rB;
        simulatedServerObj->getNodeRemainingResource(NODEA, rA);
        simulatedServerObj->getNodeRemainingResource(NODEB, rB);
        int nodeIndex;
        if (CalDistance({(double) rA.cpuNum, (double) rA.memorySize}) >
            CalDistance({(double)rB.cpuNum, (double)rB.memorySize})) {
            nodeIndex = NODEB;
        } else {
            nodeIndex = NODEA;
        }
        for (auto vmMapIt:simulatedServerObj->vmObjMap) {
            VMObj *vmObj = vmMapIt.second;
            if (vmObj->info.doubleNode != 1 && vmObj->deployNodes[0] == nodeIndex) {
                cloudOperator.markMigratedVMObj(simulatedServerObj, vmMapIt.second);
                cloudOperator.delVMObjInFakeServerObj(simulatedServerObj, vmMapIt.second->id);
                unhandledVMObj.push_back(vmMapIt.second);
                availableMigrateTime--;
                break;
            }
        }
        if (availableMigrateTime == 0) {
            return 0;
        }
        if (preMigrateTime == availableMigrateTime) {
            return 0;
        }
    }

    return 0;
}
