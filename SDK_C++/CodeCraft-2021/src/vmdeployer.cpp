//
// Created by ctoast on 2021/3/24.
//

#include "vmdeployer.h"
#include "strategytools.h"
#include "global.h"
#include "algorithm"

#define DEPLOYER_USAGESTATE_r0 0.3

std::vector<ServerObj*> DeployerServerList;

int deployServerCmp(ServerObj* s1, ServerObj* s2){
    double ful1=CalculateFullness(s1);
    double ful2=CalculateFullness(s2);
    return ful1>ful2;
//    return s1->info.energyCost<s2->info.energyCost;
}

int VMDeployer::deploy(std::vector<VMObj *> &unhandledVMObj) {
//    auto Cmp=[](const VMObj* vm1,const VMObj* vm2){
//        return vm1->info.cpuNum+vm1->info.memorySize > vm2->info.cpuNum+vm2->info.memorySize;
//    };
//    std::sort(unhandledVMObj.begin(),unhandledVMObj.end(),Cmp);
//
    DeployerServerList=globalCloud->serverObjList;
    std::sort(DeployerServerList.begin(), DeployerServerList.end(), deployServerCmp);
//
//    deployByAcceptableUsageState(unhandledVMObj, 1);
//    deployByAcceptableUsageState(unhandledVMObj, 0.8);
//    deployByAcceptableUsageState(unhandledVMObj, 0.5);
//    forceDeploy(unhandledVMObj);

    std::vector<DoubleNodeVMWrapper> unhandledDoubleVMObj;
    separateUnhandledVM(unhandledVMObj,unhandledDoubleVMObj);
    deployDoubleNodeVM(unhandledDoubleVMObj);
    deploySingleNodeVM(unhandledVMObj);
    aggregateUnhandledVM(unhandledVMObj,unhandledDoubleVMObj);
    return 0;
}

bool isSimiliar(VMInfo& info1,VMInfo& info2){
    double cpu1=info1.cpuNum;
    double mem1=info1.memorySize;
    double cpu2=info2.cpuNum;
    double mem2=info2.memorySize;
    double dis=CalDistance({cpu1,mem1},{cpu2,mem2});
    //LOGE("{%f,%f},{%f,%f}",cpu1,mem1,cpu2,mem2);
    if(dis<15){
        return true;
    }
    return false;
}

int separateUnhandledVM(std::vector<VMObj *> &unhandledSingleVMObj,std::vector<DoubleNodeVMWrapper>& unhandledDoubleNodeVMObj){
    std::vector<VMObj *> singleVMObj;
    std::map<VMObj*,bool> haveBeenPair;
    for(int i=0;i<unhandledSingleVMObj.size();i++){
        if(unhandledSingleVMObj[i]->info.doubleNode==1){
//            DoubleNodeVMWrapper w(unhandledSingleVMObj[i],NULL, false);
//            unhandledDoubleNodeVMObj.push_back(w);
            singleVMObj.push_back(unhandledSingleVMObj[i]);
        }else{
            if(haveBeenPair.find(unhandledSingleVMObj[i])!=haveBeenPair.end()){
                continue;;
            }
            std::string model=unhandledSingleVMObj[i]->info.model;
            long unsigned int j=i+1;
            for(;j<unhandledSingleVMObj.size();j++){
                if(unhandledSingleVMObj[j]->info.doubleNode!=1&&
                haveBeenPair.find(unhandledSingleVMObj[j])==haveBeenPair.end()
                &&isSimiliar(unhandledSingleVMObj[i]->info,unhandledSingleVMObj[j]->info)){
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
    return 0;
}

int aggregateUnhandledVM(std::vector<VMObj *> &unhandledSingleVMObj,std::vector<DoubleNodeVMWrapper>& unhandledDoubleNodeVMObj) {
    for (auto &it:unhandledDoubleNodeVMObj) {
        unhandledSingleVMObj.push_back(it.vm1);
        if (it.isPair) {
            unhandledSingleVMObj.push_back(it.vm2);
        }
//        if(it.isPair&&(it.vm1->id==1000580586||it.vm2->id==1000580586)){
//            LOGE("break");
//        }
    }
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
    auto Cmp = [](const DoubleNodeVMWrapper& wp1, const DoubleNodeVMWrapper& wp2) {
        int res1,res2;
        VMInfo info1=wp1.vm1->info;
        VMInfo info2=wp2.vm1->info;
        res1=info1.cpuNum+info1.memorySize;
        res2=info2.cpuNum+info2.memorySize;
        if(wp1.isPair){
            res1=res1*2+wp1.vm2->info.cpuNum+wp1.vm2->info.memorySize;
        }
        if(wp2.isPair){
            res2=res2*2+wp2.vm2->info.cpuNum+wp2.vm2->info.memorySize;
        }
        return res1 > res2;
    };
    std::sort(unhandledDoubleVMObj.begin(), unhandledDoubleVMObj.end(), Cmp);
//    for(auto &wp:unhandledDoubleVMObj){
//        if(wp.isPair){
//            LOGE("%d %d",wp.vm1->id,wp.vm2->id);
//        }
//        else{
//            LOGE("%d",wp.vm1->id);
//        }
//    }

    deployByAcceptableUsageState(unhandledDoubleVMObj, 1);
    deployByAcceptableUsageState(unhandledDoubleVMObj, 0.8);
    deployByAcceptableUsageState(unhandledDoubleVMObj, 0.5);
    forceDeploy(unhandledDoubleVMObj);
    return 0;
}

int VMDeployer::deploySingleNodeVM(std::vector<VMObj *> &unhandledSingleVMObj) {
    auto Cmp = [](const VMObj *vm1, const VMObj *vm2) {
        return vm1->info.cpuNum + vm1->info.memorySize > vm2->info.cpuNum + vm2->info.memorySize;
    };
    std::sort(unhandledSingleVMObj.begin(), unhandledSingleVMObj.end(), Cmp);

    deployByAcceptableUsageState(unhandledSingleVMObj, 1);
    deployByAcceptableUsageState(unhandledSingleVMObj, 0.8);
    deployByAcceptableUsageState(unhandledSingleVMObj, 0.5);
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
            for (auto &it:DeployerServerList) {
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
            VMInfo vmInfo1 = vmObj1->info;VMInfo vmInfo2 = vmObj2->info;
            bool haveDeploy = false;
            for (auto &it:DeployerServerList) {
                std::string serverModel = it->info.model;
                if (it->canDeployOnSingleNode(NODEA,vmInfo1)&&it->canDeployOnSingleNode(NODEB,vmInfo2)) {
                    ServerObj tmpObj = *it;
                    cloudOperator.deployVMObjInFakeServerObj(&tmpObj, vmObj1, NODEA);
                    cloudOperator.deployVMObjInFakeServerObj(&tmpObj, vmObj2, NODEB);
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
    return 0;
}

int VMDeployer::forceDeploy(std::vector<DoubleNodeVMWrapper> &unhandledDoubleVMObj) {
    std::vector<DoubleNodeVMWrapper> tmpdoubleVMSet;
    for (auto wrapper:unhandledDoubleVMObj) {
        if (!wrapper.isPair) {//simple double node vm case
            VMObj *vmObj = wrapper.vm1;
            VMInfo vmInfo = vmObj->info;
            bool haveDeploy = false;
            for (auto &it:DeployerServerList) {
                std::string serverModel = it->info.model;
                if (it->canDeployOnDoubleNode(vmInfo)) {
                    cloudOperator.deployVMObj(it->id, NODEAB, vmObj);
                    haveDeploy = true;
                    break;
                }
            }
            if (!haveDeploy) {
                tmpdoubleVMSet.push_back(wrapper);
            }
        } else {//pair case
            VMObj *vmObj1 = wrapper.vm1;VMObj *vmObj2 = wrapper.vm2;
            VMInfo vmInfo1 = vmObj1->info;VMInfo vmInfo2 = vmObj2->info;
            bool haveDeploy = false;
            for (auto &it:DeployerServerList) {
                std::string serverModel = it->info.model;
                if (it->canDeployOnSingleNode(NODEA,vmInfo1)&&it->canDeployOnSingleNode(NODEB,vmInfo2)) {
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


