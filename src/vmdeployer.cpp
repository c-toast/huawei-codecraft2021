//
// Created by ctoast on 2021/3/24.
//

#include "vmdeployer.h"
#include "strategytools.h"
#include "global.h"
#include "algorithm"

int VMDeployer::deploy(std::vector<VMObj *> &unhandledVMObj) {
    deployByFitness(unhandledVMObj);
}


int VMDeployer::deployByFitness(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpAddReqSet;

    auto Cmp=[](const VMObj* vm1,const VMObj* vm2){
        return vm1->info.cpuNum+vm1->info.memorySize > vm2->info.cpuNum+vm2->info.memorySize;
    };
    std::sort(unhandledVMObj.begin(),unhandledVMObj.end(),Cmp);

    for(auto vmObj:unhandledVMObj){
        VMInfo vmInfo=vmObj->info;
        bool haveDeploy=false;
        for(auto& it:globalCloud->serverObjList){
            std::string serverModel=it->info.model;
            int deployNode;
            if(it->canDeploy(vmInfo,deployNode)){
                ServerObj tmpObj=*it;
                cloudOperator.deployVMObjInFakeServerObj(&tmpObj,vmObj,deployNode);
                //tmpObj.deployVM(deployNode,vmObj);
                if(isDeployDecisionBetter(it, &tmpObj)){
                    cloudOperator.deployVMObj(it->id, deployNode, vmObj);
                    haveDeploy=true;
                    break;
                }
            }
        }
        if(!haveDeploy){
            tmpAddReqSet.push_back(vmObj);
        }
    }
    unhandledVMObj=tmpAddReqSet;
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

