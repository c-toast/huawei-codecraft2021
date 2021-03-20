//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "cloud.h"
#include "utils.h"
#include "cstring"
#include <cmath>
#include <algorithm>

extern SimpleCloud* globalCloud;
ResultRecorder rr;
OldServerDeployer osd;
NewServerDeployer nsd;

std::map<std::string,std::vector<std::string>> fitnessRangeMap; //[VMmodel]range
std::map<std::string,std::map<std::string,int>> fitnessMap; //[VMmodel][serverModel]fitnessRange
int ACCEPT_RANGE=(globalCloud->serverInfoMap.size())/2;

int Strategy::dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) {
    int dayNum=requestsBunch.dayNum;
    auto& bunch=requestsBunch.bunch;

    for(int i=0;i<dayNum;i++){
        OneDayResult oneDayRes;
        OneDayRequest& oneDayReq=bunch[i];

        std::vector<VMObj *> unhandledVMObj;
        std::vector<Request> unhandledDelReqSet;
        std::vector<Request> unhandledAddReqSet;

        for(auto it:oneDayReq){
            if(it.op==ADD){
                auto vmObj=globalCloud->createVMObj(it.vMachineID,it.vMachineModel);
                unhandledVMObj.push_back(vmObj);
                unhandledAddReqSet.push_back(it);
            }else{
                unhandledDelReqSet.push_back(it);
            }
        }
        osd.Deploy(unhandledVMObj);
        nsd.buyAndDeploy(unhandledVMObj);
        rr.ouputOneDayRes(unhandledAddReqSet,oneDayRes);

        for(auto it:unhandledDelReqSet){
            HandleDel(it,oneDayRes);
        }
        receiver.push_back(oneDayRes);
    }
    return 0;
}

int Strategy::HandleDel(Request &del, OneDayResult &receiver) {
    int machineId=del.vMachineID;
    globalCloud->delVMObj(machineId);
    return 0;
}

int Strategy::CalFitness(ServerInfo &serverInfo, VMInfo &vmInfo, double &fitnessReceiver) {
    int serverCpuNum=serverInfo.cpuNum;
    int serverMemSize=serverInfo.memorySize;
    int vmCpuNum=vmInfo.cpuNum;
    int vmMemSize=vmInfo.memorySize;

    double cpuNumRadio=((double)vmCpuNum)/serverCpuNum;
    double memSizeRadio=((double)vmMemSize)/serverMemSize;
    double average=(cpuNumRadio+memSizeRadio)/2;
    fitnessReceiver=(pow(cpuNumRadio-average,2)+pow(memSizeRadio-average,2))/2;
    return 0;
}

int Strategy::init() {

    auto serverInfoMap= globalCloud->serverInfoMap;
    auto vmInfoMap=globalCloud->vmInfoMap;
    struct FitRecord{
        std::string serverModel;
        double fitness;
    };

    auto reqSetCmp=[](const FitRecord& r1,const FitRecord& r2){
        return r1.fitness<r2.fitness;
    };

    for(auto &vmInfoMapIt:vmInfoMap){
        std::vector<FitRecord> fitVec;
        for(auto &serverInfoListIt:serverInfoMap){
            FitRecord r;
            r.serverModel=serverInfoListIt.second.model;
            CalFitness(serverInfoListIt.second,vmInfoMapIt.second,r.fitness);
            fitVec.push_back(r);
        }
        std::sort(fitVec.begin(),fitVec.end(),reqSetCmp);
        int i=0;
        for(auto& recordIt:fitVec){
            fitnessRangeMap[vmInfoMapIt.first].push_back(recordIt.serverModel);
            fitnessMap[vmInfoMapIt.first][recordIt.serverModel]=i;
            i++;
        }
    }

    return 0;
}

int OldServerDeployer::Deploy(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpAddReqSet;
    for(auto vmObj:unhandledVMObj){
        VMInfo vmInfo=vmObj->info;
        ServerObj* DeployableServerObj=NULL;
        int minFitNessRange=ACCEPT_RANGE;

        for(auto& it:globalCloud->serverObjList){
            std::string serverModel=it->info.model;
            int deployNode;

            if(it->canDeploy(vmInfo,deployNode)){
                int fit=fitnessMap[vmObj->info.model][serverModel];
                if(fit <= minFitNessRange){
                    minFitNessRange=fit;
                    DeployableServerObj=it;
                }
            }
        }

        if(DeployableServerObj!=NULL){
            int deployNode;
            DeployableServerObj->canDeploy(vmInfo,deployNode);
            globalCloud->deployVMObj(DeployableServerObj->id, deployNode, vmObj);
        }
        else{
            tmpAddReqSet.push_back(vmObj);
        }
    }
    unhandledVMObj=tmpAddReqSet;
    return 0;
}

int NewServerDeployer::buyAndDeploy(std::vector<VMObj *> &unhandledVMObj) {
    auto& serverObjList=globalCloud->serverObjList;
    int oldSize=serverObjList.size();
    for(auto vmObj:unhandledVMObj){
        VMInfo vmInfo=vmObj->info;
        ServerObj* DeployableServerObj=NULL;
        int minFitNessRange=ACCEPT_RANGE;

        for(int i=oldSize;i<serverObjList.size();i++) {
            int deployNode;
            ServerObj* serverObj = serverObjList[i];
            if (serverObj->canDeploy(vmInfo, deployNode)) {
                double fit = fitnessMap[vmInfo.model][serverObj->info.model];
                if (fit <= minFitNessRange) {
                    minFitNessRange = fit;
                    DeployableServerObj = serverObj;
                }
            }
        }

        if(DeployableServerObj!=NULL){
            int deployNode;
            DeployableServerObj->canDeploy(vmInfo,deployNode);
            globalCloud->deployVMObj(DeployableServerObj->id, deployNode, vmObj);
            continue;
        }

        for(int i=0;;i++) {
            std::string serverModel = fitnessRangeMap[vmInfo.model][i];
            ServerInfo serverInfo=globalCloud->serverInfoMap[serverModel];
            if(!serverInfo.canDeployOnSingleNode(vmInfo)&&!serverInfo.canDeployOnDoubleNode(vmInfo)){
                continue;
            }
            int newId=globalCloud->createServerObj(serverInfo);
            int deployNode;
            globalCloud->serverObjList[newId]->canDeploy(vmInfo,deployNode);
            globalCloud->deployVMObj(newId, deployNode, vmObj);

            break;
        }
    }

    return 0;
}

int ResultRecorder::ouputOneDayRes(std::vector<Request> addReqVec, OneDayResult &receiver) {
    auto objSetCmp=[](const ServerObj* s1,const ServerObj* s2){
        return s1->info.model<s2->info.model;
    };
    auto& serverObjList=globalCloud->serverObjList;
    if(serverObjList.size()>oldSize){
        std::sort(serverObjList.begin()+oldSize, serverObjList.end(), objSetCmp);
    }

    globalCloud->renewServerID(oldSize);

    for(int i=oldSize; i < serverObjList.size();){
        std::string model=serverObjList[i]->info.model;
        Purchase p;
        p.serverName=model;
        p.num=1;
        int j=i+1;
        for(;j<serverObjList.size();j++){
            if(serverObjList[j]->info.model==model){
                p.num++;
            }else{
                break;
            }
        }
        i=j;
        receiver.purchaseVec.push_back(p);
    }

    for(auto& it:addReqVec){
        Deploy d;
        VMObj* obj=globalCloud->vmObjMap[it.vMachineID];
        d.serverID= obj->deployServerID;
        if(obj->info.doubleNode==1){
            d.node=NODEAB;
        }else{
            d.node=obj->deployNodes[0];
        }
        receiver.deployList.push_back(d);
    }
    oldSize=serverObjList.size();

    return 0;
}
