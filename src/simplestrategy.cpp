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


int SimpleStrategy::dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) {
    int dayNum=requestsBunch.dayNum;
    auto& bunch=requestsBunch.bunch;

    for(int i=0;i<dayNum;i++){
        purchaseVec.clear();
        OneDayResult oneDayRes;
        OneDayRequest& oneDayReq=bunch[i];

        std::vector<Request> unhandledAddReqSet;
        std::vector<Request> unhandledDelReqSet;

        for(auto it:oneDayReq){
            if(it.op==ADD){
                unhandledAddReqSet.push_back(it);
            }else{
                unhandledDelReqSet.push_back(it);
            }
        }
        AddVMInOldServer(unhandledAddReqSet,oneDayRes);
        AddVMInNewServer(unhandledAddReqSet,oneDayRes);
        for(auto it:unhandledDelReqSet){
            HandleDel(it,oneDayRes);
        }

        receiver.push_back(oneDayRes);
    }
    return 0;
}

int SimpleStrategy::HandleAdd(Request &req, OneDayResult &receiver) {
    auto& serverInfoList=globalCloud->serverInfoList;
    auto& vMachineInfoMap= globalCloud->vmInfoMap;
    auto& serverObjList=globalCloud->serverObjList;
    auto model=req.vMachineModel;

    auto InfoIt=vMachineInfoMap.find(model);
    if(InfoIt==vMachineInfoMap.end()){
        LOGE("machine model does not exist");
        exit(-1);
    }

    VMInfo machineInfo= InfoIt->second;
    auto machineObj=VMObj(machineInfo, req.vMachineID);
    int vmID=req.vMachineID;
    Deploy res;

    for(int i=0;i<serverObjList.size();i++){
        auto serverObj=serverObjList[i];
        int nodeIndex;
        if(serverObj.canDeployOnSingleNode(0,machineInfo)){
            globalCloud->addVMObj(i, 0, model, vmID);
            res.serverID=i;
            res.node=0;
            receiver.deployList.push_back(res);
            return 0;
        }
        if(serverObj.canDeployOnSingleNode(1,machineInfo)){
            globalCloud->addVMObj(i, 1, model, vmID);
            res.serverID=i;
            res.node=1;
            receiver.deployList.push_back(res);
            return 0;
        }
        if(serverObj.canDeployOnDoubleNode(machineInfo)){
            globalCloud->addVMObj(i, 1, model, vmID);
            res.serverID=i;
            res.node=-1;
            receiver.deployList.push_back(res);
            return 0;
        }
    }

    //no existed obj is suitable. have to buy new server
    for(int i=0;i<serverInfoList.size();i++){
        auto serverInfo=serverInfoList[i];
        int useless;
        if(serverInfo.canDeployOnSingleNode(machineInfo)||serverInfo.canDeployOnDoubleNode(machineInfo)){
            globalCloud->addServerObj(serverInfo);
            int serverId=globalCloud->serverObjList.size()-1;
            std::string serverModel;
            serverInfo.getModel(serverModel);
            receiver.purchaseMap[serverModel]+=1;

            globalCloud->addVMObj(serverId, 0, model, vmID);
            res.serverID=serverId;
            int doubleNode;machineInfo.getDoubleNode(doubleNode);
            if(doubleNode==1){
                res.node=-1;
            }else{
                res.node=0;
            }
            receiver.deployList.push_back(res);
            return 0;
        }
    }

    return 0;
}


int SimpleStrategy::HandleDel(Request &del, OneDayResult &receiver) {
    int machineId=del.vMachineID;
    globalCloud->delVMObj(machineId);
    return 0;
}

int SimpleStrategy::AddVMInOldServer(std::vector<Request> &unhandledAddReqSet, OneDayResult &receiver) {
    auto& serverObjList=globalCloud->serverObjList;
    int infoSize=globalCloud->serverInfoList.size();
    std::vector<Request> tmpAddReqSet;

    auto reqSetCmp=[](const Request& r1,const Request& r2){
        VMInfo i1,i2;
        int cpu1,cpu2,mem1,mem2;
        globalCloud->getVMInfoByModel(r1.vMachineModel,i1);
        globalCloud->getVMInfoByModel(r2.vMachineModel,i2);
        i1.getCpuNum(cpu1);i1.getMemorySize(mem1);
        i2.getCpuNum(cpu2);i2.getMemorySize(mem2);
        return cpu1+mem1>cpu2+mem2;
    };
    std::sort(unhandledAddReqSet.begin(),unhandledAddReqSet.end(),reqSetCmp);

    for(auto& req:unhandledAddReqSet){
        VMInfo vmInfo;
        globalCloud->getVMInfoByModel(req.vMachineModel,vmInfo);
        ServerObj* DeployableServerObj=NULL;
        double minFitNess=(globalCloud->serverInfoList.size())/2;

        for(auto& it:serverObjList){
            std::string serverModel;
            int deployNode;
            it.info.getModel(serverModel);
            if(it.canDeploy(vmInfo,deployNode)){
                double fit=fitnessMap[req.vMachineModel][serverModel];
                if(fit<minFitNess){
                    minFitNess=fit;
                    DeployableServerObj=&it;
                }
            }
        }

        if(DeployableServerObj!=NULL){
            int deployNode;
            DeployableServerObj->canDeploy(vmInfo,deployNode);
            globalCloud->addVMObj(DeployableServerObj->ID,deployNode,req.vMachineModel,req.vMachineID);

            Deploy res;
            res.serverID=DeployableServerObj->ID;
            res.node=deployNode;
            receiver.deployList.push_back(res);
        }
        else{
            tmpAddReqSet.push_back(req);
        }
    }
    unhandledAddReqSet=tmpAddReqSet;
    return 0;
}

int SimpleStrategy::CalFitness(ServerInfo &serverInfo, VMInfo &vmInfo, double &fitnessReceiver) {
    int serverCpuNum,serverMemSize,vmCpuNum,vmMemSize;
    serverInfo.getCpuNum(serverCpuNum);
    serverInfo.getMemorySize(serverMemSize);
    vmInfo.getCpuNum(vmCpuNum);
    vmInfo.getMemorySize(vmMemSize);

    double cpuNumRadio=((double)vmCpuNum)/serverCpuNum;
    double memSizeRadio=((double)vmMemSize)/serverMemSize;
    double average=(cpuNumRadio+memSizeRadio)/2;
    fitnessReceiver=(pow(cpuNumRadio-average,2)+pow(memSizeRadio-average,2))/2;
    return 0;
}

int SimpleStrategy::init() {

    auto serverInfoList= globalCloud->serverInfoList;
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
        for(auto &serverInfoListIt:serverInfoList){
            FitRecord r;
            serverInfoListIt.getModel(r.serverModel);
            CalFitness(serverInfoListIt,vmInfoMapIt.second,r.fitness);
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

int SimpleStrategy::AddVMInNewServer(std::vector<Request> &unhandledAddReqSet, OneDayResult &receiver) {
    auto& serverInfoList = globalCloud->serverInfoList;
    int newObjStartIndex=globalCloud->serverObjList.size();

    for(auto& req:unhandledAddReqSet){
        ServerObj* DeployableServerObj=NULL;
        double minFitNess=(globalCloud->serverInfoList.size())/2;
        VMInfo vmInfo;
        for(int i=newObjStartIndex;i<globalCloud->serverObjList.size();i++){
            globalCloud->getVMInfoByModel(req.vMachineModel,vmInfo);
            std::string serverModel;
            int deployNode;
            ServerObj serverObj=globalCloud->serverObjList[i];
            serverObj.info.getModel(serverModel);
            if(serverObj.canDeploy(vmInfo,deployNode)){
                double fit=fitnessMap[req.vMachineModel][serverModel];
                if(fit<minFitNess){
                    minFitNess=fit;
                    DeployableServerObj=&serverObj;
                }
            }
        }
        if(DeployableServerObj!=NULL){
            int deployNode;
            DeployableServerObj->canDeploy(vmInfo,deployNode);
            globalCloud->addVMObj(DeployableServerObj->ID,deployNode,req.vMachineModel,req.vMachineID);

            Deploy res;
            res.serverID=DeployableServerObj->ID;
            res.node=deployNode;
            receiver.deployList.push_back(res);
            continue;
        }

        for(int i=0;;i++) {
            std::string serverModel = fitnessRangeMap[req.vMachineModel][i];
            ServerInfo serverInfo;
            globalCloud->getServerInfoByModel(serverModel, serverInfo);
            globalCloud->getVMInfoByModel(req.vMachineModel,vmInfo);

            if(!serverInfo.canDeployOnSingleNode(vmInfo)&&!serverInfo.canDeployOnDoubleNode(vmInfo)){
                continue;
            }
            globalCloud->addServerObj(serverInfo);
            purchaseVec.push_back(serverModel);
            globalCloud->addVMObj(globalCloud->serverObjList.size()-1,NODEA,req.vMachineModel,req.vMachineID);

            int doubleNode;
            vmInfo.getDoubleNode(doubleNode);
            Deploy res;
            res.serverID=globalCloud->serverObjList.size()-1;
            res.node=(doubleNode==1)?NODEAB:NODEA;
            receiver.deployList.push_back(res);
            break;
        }
    }

    return 0;
}



