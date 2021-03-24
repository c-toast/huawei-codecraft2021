//
// Created by ctoast on 2021/3/24.
//

#include "vmdeployer.h"
#include "strategytools.h"

int VMDeployer::migrateAndDeploy(std::vector<VMObj *> &unhandledVMObj) {
    migrate(unhandledVMObj);
    Deploy(unhandledVMObj);
}

int VMDeployer::migrate(std::vector<VMObj *> &unhandledVMObj) {
    int time=0;
    if(time>= (deployVMNum * 5) / 1000){
        return 0;
    }
    for(auto serverIt:globalCloud->serverObjList){
        int counter=0;
        ServerObj tmpserverIt=*serverIt;
        for(; !isServerInSD(&tmpserverIt, USAGESTATERO) && counter < 20;){
            for(auto vmMapIt:tmpserverIt.vmObjMap){
                counter++;
                std::string vmModel=vmMapIt.second->info.model;
                int range=fitnessMap[vmModel][tmpserverIt.info.model];
                if(range>ACCEPT_RANGE){
                    rr.migrateVMObj(&tmpserverIt, vmMapIt.second);
                    tmpserverIt.delVM(vmMapIt.second->id);//
                    unhandledVMObj.push_back(vmMapIt.second);
                    time++;
                    break;
                }
            }
            if(time>= (deployVMNum * 5) / 1000){
                return 0;
            }
        }
//        for(;!isServerNodeBalance(tmpserverIt,NODEBANLANCESTATERO);){
//            Resource rA,rB;
//            tmpserverIt->getNodeRemainingResource(NODEA,rA);
//            tmpserverIt->getNodeRemainingResource(NODEB,rB);
//            int nodeIndex;
//            if(distance(rA.cpuNum,rA.memorySize,0,0)>distance(rB.cpuNum,rB.memorySize,0,0)){
//                nodeIndex=NODEB;
//            }else{
//                nodeIndex=NODEA;
//            }
//
//            for(auto vmMapIt:tmpserverIt->vmObjMap){
//                VMObj* vmObj=vmMapIt.second;
//                if(vmObj->info.doubleNode!=1&&vmObj->deployNodes[0]==nodeIndex){
//                    rr.migrateVMObj(tmpserverIt,vmMapIt.second);
//                    unhandledVMObj.push_back(vmMapIt.second);
//                    time++;
//                    break;
//                }
//            }
//            if(time>= (deployVMNum * 5) / 1000){
//                return 0;
//            }
//        }
    }

    return 0;
}

int VMDeployer::Deploy(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpAddReqSet;

    for(auto vmObj:unhandledVMObj){
        VMInfo vmInfo=vmObj->info;
        bool haveDeploy=false;
        for(auto& it:globalCloud->serverObjList){
            std::string serverModel=it->info.model;
            int deployNode;
            if(it->canDeploy(vmInfo,deployNode)){
                ServerObj tmpObj=*it;
                tmpObj.deployVM(deployNode,vmObj);
                if(isDeployDecisionBetter(it, &tmpObj)){
                    rr.deployVMObj(it->id, deployNode, vmObj);
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
                rr.deployVMObj(it->id, deployNode, vmObj);
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

