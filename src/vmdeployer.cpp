//
// Created by ctoast on 2021/3/24.
//

#include "vmdeployer.h"
#include "strategytools.h"
#include "global.h"
#include "algorithm"

#define DEPLOYER_USAGESTATE_r0 0.3


int deployServerCmp(ServerObj *s1, ServerObj *s2) {
    double ful1 = CalculateFullness(s1);
    double ful2 = CalculateFullness(s2);
    return ful1 > ful2;
//    return s1->info.energyCost<s2->info.energyCost;
}

int VMDeployer::deploy(std::vector<VMObj *> &unhandledVMObj) {
    std::sort(unhandledVMObj.begin(), unhandledVMObj.end(), vmObjResMagnitudeCmp);

    DeployerServerList = globalCloud->serverObjList;
    std::sort(DeployerServerList.begin(), DeployerServerList.end(), deployServerCmp);

    deployByFitness(unhandledVMObj,0,10);
//    deployByFitness(unhandledVMObj,10,20);
//    deployByFitness(unhandledVMObj,20,30);
//    deployByAcceptableUsageState(unhandledVMObj, 1);

    deployByAcceptableUsageState(unhandledVMObj, 0.8);
//    deployByAcceptableUsageState(unhandledVMObj, 0.5);

    forceDeploy(unhandledVMObj);

    return 0;
}

int VMDeployer::deployByAcceptableUsageState(std::vector<VMObj *> &unhandledVMObj, double acceptableR0) {
    std::vector<VMObj *> tmpAddReqSet;

    for (auto vmObj:unhandledVMObj) {
        VMInfo vmInfo = vmObj->info;
        bool haveDeploy = false;

        for (auto &it:DeployerServerList) {
            std::string serverModel = it->info.model;
            int deployNode;
            ServerObj tmpObj;

            if (getFakeServerAndJudgeDeployable(it, tmpObj, vmObj, deployNode)) {
                cloudOperator.deployVMObjInFakeServerObj(&tmpObj, vmObj, deployNode);
                if (UsageState::isServerNodeInASD(&tmpObj, deployNode, acceptableR0, DEPLOYER_USAGESTATE_r0)) {
                    if (cloudOperator.deployVMObj(it->id, deployNode, vmObj) < 0) {
                        continue;
                    }
                    haveDeploy = true;
                    break;
                }
            }
        }
        if (!haveDeploy) {
            tmpAddReqSet.push_back(vmObj);
        }
    }
    unhandledVMObj = tmpAddReqSet;
    return 0;
}

int VMDeployer::forceDeploy(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpAddReqSet;
    tmpAddReqSet.clear();

    for (auto vmObj:unhandledVMObj) {
        VMInfo vmInfo = vmObj->info;
        bool haveDeploy = false;


        for (auto &it:DeployerServerList) {
            std::string serverModel = it->info.model;
            int deployNode;
            ServerObj tmpObj;
            if (getFakeServerAndJudgeDeployable(it, tmpObj, vmObj, deployNode)) {
                if (cloudOperator.deployVMObj(it->id, deployNode, vmObj) < 0) {
                    continue;
                }
                haveDeploy = true;
                break;
            }
        }
        if (!haveDeploy) {
            tmpAddReqSet.push_back(vmObj);
        }
    }

    unhandledVMObj = tmpAddReqSet;
    return 0;

}

int VMDeployer::deployByFitness(std::vector<VMObj *> &unhandledVMObj,int start,int end) {
    std::vector<VMObj *> tmpAddReqSet;

    for (auto vmObj:unhandledVMObj) {
        VMInfo vmInfo = vmObj->info;
        bool haveDeploy = false;

        for(int i=start;i<end;i++){
            std::string serverModel=fitnessRangeMap[vmInfo.model][i];
            for(auto &it:globalCloud->modelServerMap[serverModel]){
                int deployNode;
                ServerObj tmpObj;
                if (getFakeServerAndJudgeDeployable(it, tmpObj, vmObj, deployNode)) {
                    if (cloudOperator.deployVMObj(it->id, deployNode, vmObj) < 0) {
                        continue;
                    }
                    haveDeploy = true;
                    break;
                }
            }
            if(haveDeploy){
                break;
            }
        }

        if (!haveDeploy) {
            tmpAddReqSet.push_back(vmObj);
        }
    }
    unhandledVMObj = tmpAddReqSet;
    return 0;
}

int VMDeployer::getFakeServerForVM(ServerObj *serverObj, VMObj *vmObj, ServerObj &receiver) {
    int time = 0;
    //new
    if (cloudOperator.migrationMap.find(vmObj) == cloudOperator.migrationMap.end()) {
        time = cloudOperator.vmReqTimeMap[vmObj];
    }
    return cloudOperator.getFakeServerObj(serverObj, receiver, time);
}

bool VMDeployer::getFakeServerAndJudgeDeployable(ServerObj *serverObj, ServerObj &fakeServerObj, VMObj *vmObj,
                                                 int &nodeIndex) {
    if (!serverObj->canDeploy(vmObj->info, nodeIndex)) {
        return false;
    }
    if (getFakeServerForVM(serverObj, vmObj, fakeServerObj) < 0) {
        return false;
    }
    if (!fakeServerObj.canDeploy(vmObj->info, nodeIndex)) {
        return false;
    }
    return true;
}


