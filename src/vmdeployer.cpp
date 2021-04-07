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

    //deployByFitness(unhandledVMObj);
    deployByAcceptableUsageState(unhandledVMObj, 1);

//    deployByAcceptableUsageState(unhandledVMObj, 0.8);
//    deployByAcceptableUsageState(unhandledVMObj, 0.5);

    forceDeploy(unhandledVMObj);

    return 0;
}

int VMDeployer::deployByAcceptableUsageState(std::vector<VMObj *> &unhandledVMObj, double acceptableR0) {
    std::vector<VMObj *> tmpAddReqSet;

    for (auto vmObj:unhandledVMObj) {
        VMInfo vmInfo = vmObj->info;
        bool haveDeploy = false;

        int time = 0;
        //if the vm to be deployed is new
        if (cloudOperator.migrationMap.find(vmObj) == cloudOperator.migrationMap.end()) {
            time = cloudOperator.vmReqTimeMap[vmObj];
        }
        for (auto &it:DeployerServerList) {
            std::string serverModel = it->info.model;
            int deployNode;
            ServerObj tmpObj;

            if (it->canDeploy(vmInfo, deployNode)) {//judge first, otherwise it is too costly to copy fake server
                if (cloudOperator.getFakeServerObj(it, tmpObj, time) < 0) {
                    continue;
                }
                if (tmpObj.canDeploy(vmInfo, deployNode)) {
                    cloudOperator.deployVMObjInFakeServerObj(&tmpObj, vmObj, deployNode);
                    if (UsageState::isServerNodeInASD(&tmpObj, NODEAB, acceptableR0, DEPLOYER_USAGESTATE_r0)) {
                        if (cloudOperator.deployVMObj(it->id, deployNode, vmObj) < 0) {
                            continue;
                        }
                        haveDeploy = true;
                        break;
                    }
//                if(isDeployDecisionBetter(it, &tmpObj)){
//                    cloudOperator.deployVMObj(it->id, deployNode, vmObj);
//                    haveDeploy=true;
//                    break;
//                }
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
        int time = 0;
        //new
        if (cloudOperator.migrationMap.find(vmObj) == cloudOperator.migrationMap.end()) {
            time = cloudOperator.vmReqTimeMap[vmObj];
        }

        for (auto &it:DeployerServerList) {
            std::string serverModel = it->info.model;
            int deployNode;
            if (it->canDeploy(vmInfo, deployNode)) {
                ServerObj tmpObj;
                if (cloudOperator.getFakeServerObj(it, tmpObj, time) < 0) {
                    continue;
                }
                if (tmpObj.canDeploy(vmInfo, deployNode)) {
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

int VMDeployer::deployByFitness(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpAddReqSet;

    for (auto vmObj:unhandledVMObj) {
        VMInfo vmInfo = vmObj->info;
//        double minRange=1000;//redisual
        int minRange = 1000;
        int minRangeServerId = -1;
        int minRangeServerDeployNode = -1;
        for (auto &it:DeployerServerList) {
            std::string serverModel = it->info.model;
            int deployNode;
            if (it->canDeploy(vmInfo, deployNode)) {
                int range = fitnessMap[vmInfo.model][it->info.model];
                //double range=CalFitness(it,deployNode,vmInfo);//redisual
                if (range < minRange) {
                    minRange = range;
                    minRangeServerId = it->id;
                    minRangeServerDeployNode = deployNode;
                }
            }
        }
        if (minRangeServerId != -1) {
            cloudOperator.deployVMObj(minRangeServerId, minRangeServerDeployNode, vmObj);
        } else {
            tmpAddReqSet.push_back(vmObj);
        }
    }
    unhandledVMObj = tmpAddReqSet;
    return 0;

}


