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
#define ACCEPT_RANGE 20
#define MIGRATE_ACCEPT_FITNESS 0.00001

int VMMigrater::initWhenNewDayStart(OneDayRequest &oneDayReq) {
    availableMigrateTime = (globalCloud->vmObjMap.size() * 3) / 100;

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

//    migrateByFitness(unhandledVMObj);

    //do not migrate the vm to be delete!!
    for (auto serverIt:serverObjList) {//the server used for migrate do not have the vm to be deleted
        if (availableMigrateTime == 0) {
            return 0;
        }

        //the serverState is the main target, so it is the condition of the while loop
        //migrate by node balance, then the server may be not in ad state again, which will increase
        //the migrate next time

        while (!UsageState::isServerNodeInASD(serverIt, NODEAB, MIGRATER_USAGESTATE_R0, MIGRATER_USAGESTATE_r0)) {
            int preMigrateTime = availableMigrateTime;
            migrateByUsageState(unhandledVMObj, serverIt);
            //migrateByNodeBalance(unhandledVMObj, &tmpserverIt);
            // migrateByVMNum(unhandledVMObj,&tmpserverIt);
            if (availableMigrateTime == 0 || preMigrateTime == availableMigrateTime) {
                break;
            }
        }
    }
    return 0;
}

int VMMigrater::migrateByUsageState(std::vector<VMObj *> &unhandledVMObj, ServerObj *simulatedServerObj) {
    if (availableMigrateTime == 0) {
        return 0;
    }
    for (; !UsageState::isServerNodeInASD(simulatedServerObj, NODEAB, MIGRATER_USAGESTATE_R0,
                                          MIGRATER_USAGESTATE_r0);) {
        std::vector<VMObj *> vmObjList;
        sortServerVMObj(simulatedServerObj, vmObjList);

        for (auto vmMapIt:vmObjList) {
            std::string vmModel = vmMapIt->info.model;
            int range = fitnessRangeMap[vmModel][simulatedServerObj->info.model];
            double rangeValue = fitnessMap[vmModel][simulatedServerObj->info.model];
            if (range > ACCEPT_RANGE) {
                cloudOperator.markMigratedVMObj(simulatedServerObj, vmMapIt);
                unhandledVMObj.push_back(vmMapIt);
                availableMigrateTime--;
                break;
            }
        }
        return 0;
        //fitness version
//        for (auto vmMapIt:vmObjList) {
//            ServerObj fakeServerObj=cloudOperator.getFakeServerObj(simulatedServerObj);
//            cloudOperator.delVMObjInFakeServerObj(&fakeServerObj,vmMapIt->id);
//            double fitness;
//            if(vmMapIt->info.doubleNode==1){
//                fitness=CalFitness(&fakeServerObj,NODEAB,vmMapIt->info);
//            }else{
//                fitness=CalFitness(&fakeServerObj,NODEAB,vmMapIt->info);
//            }
//            if (fitness>MIGRATE_ACCEPT_FITNESS) {
//                cloudOperator.markMigratedVMObj(simulatedServerObj, vmMapIt);
//                cloudOperator.delVMObjInFakeServerObj(simulatedServerObj, vmMapIt->id);
//                unhandledVMObj.push_back(vmMapIt);
//                availableMigrateTime--;
//                break;
//            }
//        }

//        if (availableMigrateTime == 0) {
//            return 0;
//        }
//        if (preMigrateTime == availableMigrateTime) {
//            return 0;
//        }
    }
}

int VMMigrater::migrateByNodeBalance(std::vector<VMObj *> &unhandledVMObj, ServerObj *simulatedServerObj) {
    if (availableMigrateTime == 0) {
        return 0;
    }
    for (; !BalanceState::isServerBalanceInASD(simulatedServerObj, MIGRATER_BALANCESTATE_R0,
                                               MIGRATER_BALANCESTATE_r0);) {
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
            if (vmObj->info.doubleNode != 1 && vmObj->deployNodes[0] == nodeIndex) {
                cloudOperator.markMigratedVMObj(simulatedServerObj, vmMapIt.second);
                globalCloud->moveVMObjFromServerObj(vmMapIt.second->id);
//                cloudOperator.delVMObjInFakeServerObj(simulatedServerObj, vmMapIt.second->id);
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

int VMMigrater::sortServerVMObj(ServerObj *serverObj, std::vector<VMObj *> &receiver) {
    receiver.clear();
    for (auto vmMapIt:serverObj->vmObjMap) {
        receiver.push_back(vmMapIt.second);
    }
    sort(receiver.begin(), receiver.end(), vmObjResMagnitudeCmp);

    return 0;
}

int VMMigrater::init() {
    globalCloud->registerBeforeListener(&listener);
    return 0;
}

int VMMigrater::migrateByFitness(std::vector<VMObj *> &unhandledVMObj) {
    if (availableMigrateTime == 0) {
        return 0;
    }
    std::vector<VMObj *> candidateVMVec;
    for (auto it:listener.candidateVMMap) {
        candidateVMVec.push_back(it.second);
    }
    auto Cmp = [](const VMObj *v1, const VMObj *v2) {
        double fit1 = fitnessMap[v1->info.model][globalCloud->serverObjList[v1->deployServerID]->info.model];
        double fit2 = fitnessMap[v2->info.model][globalCloud->serverObjList[v2->deployServerID]->info.model];
        return fit1 > fit2;
    };
    std::sort(candidateVMVec.begin(),candidateVMVec.end(),Cmp);

    for (auto it:candidateVMVec) {
        ServerObj *serverObj = globalCloud->serverObjList[it->deployServerID];
        cloudOperator.markMigratedVMObj(serverObj, it);
        unhandledVMObj.push_back(it);
        availableMigrateTime--;
        if (availableMigrateTime == 0) {
            return 0;
        }
    }
    return 0;
}

int migraterListener::moveVMObjFromServerObj(int vmID) {
    auto it = candidateVMMap.find(vmID);
    if (it != candidateVMMap.end()) {
        candidateVMMap.erase(it);
    }
    return 0;
}

int migraterListener::deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj) {
    std::string serverModel = globalCloud->serverObjList[serverObjID]->info.model;
    if (fitnessMap[vmObj->info.model][serverModel] < 0.25) {
        candidateVMMap.insert({vmObj->id, vmObj});
    }
    return 0;
}
