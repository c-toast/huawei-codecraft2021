//
// Created by ctoast on 2021/3/24.
//

#include <algorithm>
#include "vmmigrater.h"
#include "global.h"

#define MIGRATER_USAGESTATE_r0 0.3
#define MIGRATER_BALANCESTATE_r0 0.3
#define MIGRATER_USAGESTATE_R0 1
#define MIGRATER_BALANCESTATE_R0 1
#define ACCEPT_RANGE 5

int VMMigrater::initWhenNewDayStart(){
    availableMigrateTime= (globalCloud->vmObjMap.size() * 5) / 1000;
    return 0;
}

int migrateServerCmp(ServerObj *s1, ServerObj *s2) {
    double ful1 = CalculateFullness(s1);
    double ful2 = CalculateFullness(s2);
    return ful1 < ful2;
//    return s1->info.energyCost>s2->info.energyCost;
}

int VMMigrater::migrate(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<ServerObj *> serverObjList = globalCloud->serverObjList;
    std::sort(serverObjList.begin(), serverObjList.end(), migrateServerCmp);

    for (auto serverIt:serverObjList) {
        if (availableMigrateTime == 0) {
            return 0;
        }
        ServerObj tmpserverIt = cloudOperator.getFakeServerObj(serverIt);
        //unknown reason, write code by this will decrease the number of migrate
        while (!UsageState::isServerNodeInASD(&tmpserverIt, NODEAB, MIGRATER_USAGESTATE_R0, MIGRATER_USAGESTATE_r0)) {
            int preMigrateTime = availableMigrateTime;
            migrateByUsageState(unhandledVMObj, &tmpserverIt);
            //migrateByNodeBalance(unhandledVMObj, &tmpserverIt);//migrateByVMNum(unhandledVMObj,&tmpserverIt);
            if (availableMigrateTime == 0 || preMigrateTime == availableMigrateTime) {
                break;
            }
        }

        //migrate by node balance, then the server may be not in ad state again, which will increase
        //the migrate next time
//        ServerObj tmpserverIt=cloudOperator.getFakeServerObj(serverIt);
//        migrateByUsageState(unhandledVMObj,&tmpserverIt);
//        migrateByNodeBalance(unhandledVMObj,&tmpserverIt);
        //migrateByVMNum(unhandledVMObj,&tmpserverIt);
    }
    return 0;
}

int VMMigrater::migrateByUsageState(std::vector<VMObj *> &unhandledVMObj, ServerObj *simulatedServerObj) {
    if (availableMigrateTime == 0) {
        return 0;
    }
    for (; !UsageState::isServerNodeInASD(simulatedServerObj, NODEAB, MIGRATER_USAGESTATE_R0, MIGRATER_USAGESTATE_r0);) {
        int preMigrateTime = availableMigrateTime;
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
        return 0;
        if (availableMigrateTime == 0) {
            return 0;
        }
        if (preMigrateTime == availableMigrateTime) {
            return 0;
        }
    }
}

int VMMigrater::migrateByNodeBalance(std::vector<VMObj *> &unhandledVMObj, ServerObj *simulatedServerObj) {
    if (availableMigrateTime == 0) {
        return 0;
    }
    for (; !BalanceState::isServerBalanceInASD(simulatedServerObj, MIGRATER_BALANCESTATE_R0, MIGRATER_BALANCESTATE_r0);) {
        int preMigrateTime = availableMigrateTime;
        Resource rA, rB;
        simulatedServerObj->getNodeRemainingResource(NODEA, rA);
        simulatedServerObj->getNodeRemainingResource(NODEB, rB);
        int nodeIndex;
        if (CalDistance({(double) rA.cpuNum, (double) rA.memorySize}) >
            CalDistance({(double) rB.cpuNum, (double) rB.memorySize})) {
            nodeIndex = NODEB;
        } else {
            nodeIndex = NODEA;
        }
        for (auto vmMapIt:simulatedServerObj->vmObjMap) {
            VMObj *vmObj = vmMapIt.second;
            if (vmObj->pairVMObj != NULL) {
                continue;
            }
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