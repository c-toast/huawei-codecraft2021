//
// Created by ctoast on 2021/3/24.
//

#include "vmdeployer.h"
#include "strategytools.h"
#include "global.h"
#include "algorithm"

#define DEPLOYER_USAGESTATE_r0 0.3

int separateUnhandledVM(std::vector<VMObj *> &unhandledSingleVMObj,std::vector<DoubleNodeVMWrapper>& unhandledDoubleNodeVMObj){
    std::vector<VMObj *> singleVMObj;
    std::map<VMObj*,bool> haveBeenPair;
    for(int i=0;i<unhandledSingleVMObj.size();i++){
        if(unhandledSingleVMObj[i]->info.doubleNode==1){
            DoubleNodeVMWrapper w(unhandledSingleVMObj[i],NULL, false);
            unhandledDoubleNodeVMObj.push_back(w);
        }else{
            if(haveBeenPair.find(unhandledSingleVMObj[i])!=haveBeenPair.end()){
                continue;;
            }
            std::string model=unhandledSingleVMObj[i]->info.model;
            int j=i+1;
            for(;j<unhandledSingleVMObj.size();j++){
                if(unhandledSingleVMObj[j]->info.model==model){
                    DoubleNodeVMWrapper w(unhandledSingleVMObj[i],unhandledSingleVMObj[j],true);
                    unhandledDoubleNodeVMObj.push_back(w);
                    haveBeenPair[unhandledSingleVMObj[j]]=true;
                    break;
                }
            }
            if(j==unhandledSingleVMObj.size()){
                singleVMObj.push_back(unhandledSingleVMObj[i]);
            }
        }
    }
    unhandledSingleVMObj=singleVMObj;
}

int aggregateUnhandledVM(std::vector<VMObj *> &unhandledSingleVMObj,std::vector<DoubleNodeVMWrapper>& unhandledDoubleNodeVMObj){
    for(auto& it:unhandledDoubleNodeVMObj){
        unhandledSingleVMObj.push_back(it.vm1);
        if(it.isPair){
            unhandledSingleVMObj.push_back(it.vm2);
        }
    }
}

int VMDeployer::deploy(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<DoubleNodeVMWrapper> unhandledDoubleVMObj;
    separateUnhandledVM(unhandledVMObj,unhandledDoubleVMObj);
    deployDoubleNodeVM(unhandledDoubleVMObj);
    deploySingleNodeVM(unhandledVMObj);
    aggregateUnhandledVM(unhandledVMObj,unhandledDoubleVMObj);
}


int VMDeployer::deployByAcceptableUsageState(std::vector<VMObj *> &unhandledVMObj, double acceptableR0) {
    std::vector<VMObj *> tmpAddReqSet;

    for (auto vmObj:unhandledVMObj) {
        VMInfo vmInfo = vmObj->info;
        bool haveDeploy = false;
        for (auto &it:globalCloud->serverObjList) {
            std::string serverModel = it->info.model;
            int deployNode;
            if (it->canDeploy(vmInfo, deployNode)) {
                ServerObj tmpObj = *it;
                cloudOperator.deployVMObjInFakeServerObj(&tmpObj, vmObj, deployNode);
                if (UsageState::isServerNodeInASD(&tmpObj, NODEAB, acceptableR0, DEPLOYER_USAGESTATE_r0)) {
                    cloudOperator.deployVMObj(it->id, deployNode, vmObj);
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
        if (!haveDeploy) {
            tmpAddReqSet.push_back(vmObj);
        }
    }
    unhandledVMObj = tmpAddReqSet;

}

int VMDeployer::forceDeploy(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpAddReqSet;
    tmpAddReqSet.clear();

    for (auto vmObj:unhandledVMObj) {
        VMInfo vmInfo = vmObj->info;
        bool haveDeploy = false;
        for (auto &it:globalCloud->serverObjList) {
            std::string serverModel = it->info.model;
            int deployNode;
            if (it->canDeploy(vmInfo, deployNode)) {
                ServerObj tmpObj = *it;
                cloudOperator.deployVMObj(it->id, deployNode, vmObj);
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

int VMDeployer::deployDoubleNodeVM(std::vector<DoubleNodeVMWrapper> &unhandledDoubleVMObj) {
    auto Cmp = [](const DoubleNodeVMWrapper& vm1, const DoubleNodeVMWrapper& vm2) {
        int res1,res2;
        VMInfo info1=vm1.vm1->info;
        VMInfo info2=vm2.vm1->info;
        res1=info1.cpuNum+info1.memorySize;
        res2=info2.cpuNum+info2.memorySize;
        if(vm1.isPair){
            res1=res1*2;
        }
        if(vm2.isPair){
            res2=res2*2;
        }
        return res1 > res2;
    };
    std::sort(unhandledDoubleVMObj.begin(), unhandledDoubleVMObj.end(), Cmp);

    deployByAcceptableUsageState(unhandledDoubleVMObj, 1);
//    deployByAcceptableUsageState(unhandledDoubleVMObj, 0.8);
//    deployByAcceptableUsageState(unhandledDoubleVMObj, 0.5);
    forceDeploy(unhandledDoubleVMObj);
    return 0;
}

int VMDeployer::deploySingleNodeVM(std::vector<VMObj *> &unhandledSingleVMObj) {
    auto Cmp = [](const VMObj *vm1, const VMObj *vm2) {
        return vm1->info.cpuNum + vm1->info.memorySize > vm2->info.cpuNum + vm2->info.memorySize;
    };
    std::sort(unhandledSingleVMObj.begin(), unhandledSingleVMObj.end(), Cmp);

    deployByAcceptableUsageState(unhandledSingleVMObj, 1);
//    deployByAcceptableUsageState(unhandledSingleVMObj, 0.8);
//    deployByAcceptableUsageState(unhandledSingleVMObj, 0.5);
    forceDeploy(unhandledSingleVMObj);
    return 0;
}

int VMDeployer::deployByAcceptableUsageState(std::vector<DoubleNodeVMWrapper> &unhandledDoubleVMObj, double acceptableR0) {
    std::vector<DoubleNodeVMWrapper> tmpdoubleVMSet;
    for (auto wrapper:unhandledDoubleVMObj) {
        if (!wrapper.isPair) {//simple double node vm case
            VMObj *vmObj = wrapper.vm1;
            VMInfo vmInfo = vmObj->info;
            bool haveDeploy = false;
            for (auto &it:globalCloud->serverObjList) {
                std::string serverModel = it->info.model;
                if (it->canDeployOnDoubleNode(vmInfo)) {
                    ServerObj tmpObj = *it;
                    cloudOperator.deployVMObjInFakeServerObj(&tmpObj, vmObj, NODEAB);
                    if (UsageState::isServerNodeInASD(&tmpObj, NODEAB, acceptableR0, DEPLOYER_USAGESTATE_r0)) {
                        cloudOperator.deployVMObj(it->id, NODEAB, vmObj);
                        haveDeploy = true;
                        break;
                    }
                }
            }
            if (!haveDeploy) {
                tmpdoubleVMSet.push_back(wrapper);
            }
        } else {//pair case
            VMObj *vmObj1 = wrapper.vm1;VMObj *vmObj2 = wrapper.vm2;
            VMInfo vmInfo = vmObj1->info;
            bool haveDeploy = false;
            for (auto &it:globalCloud->serverObjList) {
                std::string serverModel = it->info.model;
                if (it->canDeployOnSingleNode(NODEA,vmInfo)&&it->canDeployOnSingleNode(NODEB,vmInfo)) {
                    ServerObj tmpObj = *it;
                    cloudOperator.deployVMObjInFakeServerObj(&tmpObj, vmObj1, NODEA);
                    cloudOperator.deployVMObjInFakeServerObj(&tmpObj, vmObj1, NODEB);
                    if (UsageState::isServerNodeInASD(&tmpObj, NODEAB, acceptableR0, DEPLOYER_USAGESTATE_r0)) {
                        cloudOperator.deployPairVMObj(it->id, vmObj1,vmObj2);
                        haveDeploy = true;
                        break;
                    }
                }
            }
            if (!haveDeploy) {
                tmpdoubleVMSet.push_back(wrapper);
            }
        }
    }
    unhandledDoubleVMObj = tmpdoubleVMSet;

}

int VMDeployer::forceDeploy(std::vector<DoubleNodeVMWrapper> &unhandledDoubleVMObj) {
    std::vector<DoubleNodeVMWrapper> tmpdoubleVMSet;
    for (auto wrapper:unhandledDoubleVMObj) {
        if (!wrapper.isPair) {//simple double node vm case
            VMObj *vmObj = wrapper.vm1;
            VMInfo vmInfo = vmObj->info;
            bool haveDeploy = false;
            for (auto &it:globalCloud->serverObjList) {
                std::string serverModel = it->info.model;
                int deployNode;
                if (it->canDeployOnDoubleNode(vmInfo)) {
                    cloudOperator.deployVMObj(it->id, deployNode, vmObj);
                    haveDeploy = true;
                    break;
                }
            }
            if (!haveDeploy) {
                tmpdoubleVMSet.push_back(wrapper);
            }
        } else {//pair case
            VMObj *vmObj1 = wrapper.vm1;VMObj *vmObj2 = wrapper.vm2;
            VMInfo vmInfo = vmObj1->info;
            bool haveDeploy = false;
            for (auto &it:globalCloud->serverObjList) {
                std::string serverModel = it->info.model;
                if (it->canDeployOnSingleNode(NODEA,vmInfo)&&it->canDeployOnSingleNode(NODEB,vmInfo)) {
                    cloudOperator.deployPairVMObj(it->id, vmObj1,vmObj2);
                    haveDeploy = true;
                    break;
                }
            }
            if (!haveDeploy) {
                tmpdoubleVMSet.push_back(wrapper);
            }
        }
    }
    unhandledDoubleVMObj = tmpdoubleVMSet;
    return 0;

}


