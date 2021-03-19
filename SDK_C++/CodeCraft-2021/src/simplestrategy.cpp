//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "cloud.h"
#include "utils.h"
#include "cstring"

extern SimpleCloud* globalCloud;

int SimpleStrategy::dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) {
    int dayNum=requestsBunch.dayNum;
    auto& bunch=requestsBunch.bunch;

    for(int i=0;i<dayNum;i++){
        purchaseMap.clear();
        OneDayResult oneDayRes;

        OneDayRequest& oneDayReq=bunch[i];
        for(auto it:oneDayReq){
            if(it.op==ADD){
                HandleAdd(it,oneDayRes);
            }else{
                HandleDel(it,oneDayRes);
            }
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

int SimpleStrategy::AnalysisRequestBunch(RequestsBunch &requestsBunch) {
    return 0;
}

