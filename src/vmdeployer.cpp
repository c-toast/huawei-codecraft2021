//
// Created by ctoast on 2021/3/24.
//

#include "vmdeployer.h"
#include "strategytools.h"
#include "global.h"
#include "algorithm"

#define DEPLOYER_USAGESTATE_r0 0.3

int VMDeployer::deploy(std::vector<VMObj *> &unhandledVMObj) {
    auto Cmp=[](const VMObj* vm1,const VMObj* vm2){
        return vm1->info.cpuNum+vm1->info.memorySize > vm2->info.cpuNum+vm2->info.memorySize;
    };
    std::sort(unhandledVMObj.begin(),unhandledVMObj.end(),Cmp);

    deployByAcceptableUsageState(unhandledVMObj, 1);
    deployByAcceptableUsageState(unhandledVMObj, 0.8);
    deployByAcceptableUsageState(unhandledVMObj, 0.5);
    forceDeploy(unhandledVMObj);
}


int VMDeployer::deployByAcceptableUsageState(std::vector<VMObj *> &unhandledVMObj, double acceptableR0) {
    std::vector<VMObj *> tmpAddReqSet;

    for(auto vmObj:unhandledVMObj){
        VMInfo vmInfo=vmObj->info;
        bool haveDeploy=false;
        for(auto& it:globalCloud->serverObjList){
            std::string serverModel=it->info.model;
            int deployNode;
            if(it->canDeploy(vmInfo,deployNode)){
                ServerObj tmpObj=*it;
                cloudOperator.deployVMObjInFakeServerObj(&tmpObj,vmObj,deployNode);
                if(UsageState::isServerNodeInASD(&tmpObj, NODEAB, acceptableR0, DEPLOYER_USAGESTATE_r0)){
                    cloudOperator.deployVMObj(it->id, deployNode, vmObj);
                    haveDeploy=true;
                    break;
                }
//                if(isDeployDecisionBetter(it, &tmpObj)){
//                    cloudOperator.deployVMObj(it->id, deployNode, vmObj);
//                    haveDeploy=true;
//                    break;
//                }
            }
        }
        if(!haveDeploy){
            tmpAddReqSet.push_back(vmObj);
        }
    }
    unhandledVMObj=tmpAddReqSet;

}

int VMDeployer::forceDeploy(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpAddReqSet;
    tmpAddReqSet.clear();

    for(auto vmObj:unhandledVMObj){
        VMInfo vmInfo=vmObj->info;
        bool haveDeploy=false;
        for(auto& it:globalCloud->serverObjList){
            std::string serverModel=it->info.model;
            int deployNode;
            if(it->canDeploy(vmInfo,deployNode)){
                ServerObj tmpObj=*it;
                cloudOperator.deployVMObj(it->id, deployNode, vmObj);
                haveDeploy=true;
                break;
            }
        }
        if(!haveDeploy){
            tmpAddReqSet.push_back(vmObj);
        }
    }

    unhandledVMObj=tmpAddReqSet;
    return 0;

}


