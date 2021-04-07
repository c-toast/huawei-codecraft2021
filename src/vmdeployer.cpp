//
// Created by ctoast on 2021/3/24.
//

#include "vmdeployer.h"
#include "strategytools.h"
#include "global.h"
#include "algorithm"

#define DEPLOYER_USAGESTATE_r0 0.3


int deployServerCmp(ServerObj *s1, ServerObj *s2) {
    double s1n1=Resource::CalResourceMagnitude(s1->nodes[0].remainingResource);
    double s1n2=Resource::CalResourceMagnitude(s1->nodes[1].remainingResource);

    double s2n1=Resource::CalResourceMagnitude(s2->nodes[0].remainingResource);
    double s2n2=Resource::CalResourceMagnitude(s2->nodes[1].remainingResource);

    double r1=s1n1<s1n2?s1n1:s1n2;
    double r2=s2n1<s2n2?s2n1:s2n2;
    return r1<r2;

//    double ful1 = CalculateFullness(s1);
//    double ful2 = CalculateFullness(s2);
//    return ful1 > ful2;

}

int VMDeployer::deploy(std::vector<VMObj *> &unhandledVMObj) {
    std::sort(unhandledVMObj.begin(), unhandledVMObj.end(), vmObjResMagnitudeCmp);

    initServerList();

    deployByFitness(unhandledVMObj,0,10);
//    deployByFitness(unhandledVMObj,10,20);
//    deployByFitness(unhandledVMObj,20,30);
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

        int i=getStartIndexOfServerList(vmObj);
        for (;i<DeployerServerList.size();i++) {
            auto it=DeployerServerList[i];
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

        int i=getStartIndexOfServerList(vmObj);
        for (;i<DeployerServerList.size();i++) {
            auto it=DeployerServerList[i];
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

int VMDeployer::initServerList() {
    int lastIndex=globalCloud->serverObjList.size();
    serverIndex={{0,lastIndex},{10,lastIndex},{30,lastIndex},{60,lastIndex},{80,lastIndex},{100,lastIndex},{150,lastIndex},{200,lastIndex},{10000,lastIndex}};
    DeployerServerList = globalCloud->serverObjList;
    std::sort(DeployerServerList.begin(), DeployerServerList.end(), deployServerCmp);

    int j=0;
    for(int i=0;i<DeployerServerList.size()&&j<serverIndex.size();i++){
        ServerObj* s1=DeployerServerList[i];
        double s1n1=Resource::CalResourceMagnitude(s1->nodes[0].remainingResource);
        double s1n2=Resource::CalResourceMagnitude(s1->nodes[1].remainingResource);
        double r1=s1n1<s1n2?s1n1:s1n2;
        if(r1>=serverIndex[j].first){
            serverIndex[j].second=i;
            j++;
        }
    }

    return 0;
}

int VMDeployer::getStartIndexOfServerList(VMObj *vmObj) {
    Resource res;
    vmObj->info.getRequiredResourceForOneNode(res);
    int ret=0;
    double m=Resource::CalResourceMagnitude(res);
    for(int i=0;i<serverIndex.size();i++){
        if(serverIndex[i].first>m){
            ret=serverIndex[i-1].second;
            break;
        }
    }
    return ret;
}


