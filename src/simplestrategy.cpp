//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "cloud.h"
#include "utils.h"
#include "cstring"

extern SimpleCloud* globalCloud;

int SimpleStrategy::dispatch(RequestsBunch &requestsBunch, std::vector<Result> &receiver) {
    int dayNum=requestsBunch.dayNum;
    auto& bunch=requestsBunch.bunch;

    for(int i=0;i<dayNum;i++){
        purchaseMap.clear();
        Result oneDayRes;

        OneDayRequest& oneDayReq=bunch[i];
        for(auto it:oneDayReq){
            if(it.op==ADD){
                HandleAdd(it,oneDayRes);
            }else{
                HandleDel(it,oneDayRes);
            }
        }

        for(auto it:purchaseMap){
            Purchase p;
            memcpy(p.serverName,it.first.c_str(),it.first.size()+1);
            p.num=it.second;
            oneDayRes.purchaseList.push_back(p);
        }
        receiver.push_back(oneDayRes);
    }
    return 0;
}

int SimpleStrategy::HandleAdd(Request &req, Result &receiver) {
    auto& serverInfoList=globalCloud->serverInfoList;
    auto& vMachineInfoMap= globalCloud->vMachineInfoMap;
    auto& serverObjList=globalCloud->serverObjList;
    auto model=req.vMachineModel;

    std::string model_str(model);
    auto InfoIt=vMachineInfoMap.find(model_str);
    if(InfoIt==vMachineInfoMap.end()){
        LOGE("machine model does not exist");
        exit(-1);
    }

    VirtualMachineInfo machineInfo= InfoIt->second;
    auto machineObj=VirtualMachineObj(machineInfo,req.vMachineID);

    if(machineInfo.doubleNode==1){
        for(int i=0;i<serverObjList.size();i++){
            auto serverObj=serverObjList[i];
            int useless;
            if(CanServerObjDeployable(serverObj,machineInfo,useless)){
                globalCloud->AddMachine(i,useless,machineObj);
                Deploy res;
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
            if(CanServerDeployable(serverInfo,machineInfo,useless)){
                auto serverObj=ServerObj(serverInfo);
                int serverId=globalCloud->AddServerObj(serverObj);

                purchaseMap[std::string(serverInfo.model)]+=1;

                globalCloud->AddMachine(serverId,useless,machineObj);
                Deploy res;
                res.serverID=serverId;
                res.node=-1;
                receiver.deployList.push_back(res);
                return 0;
            }
        }
    }else{// single node
        for(int i=0;i<serverObjList.size();i++){
            auto serverObj=serverObjList[i];
            int useless;
            if(CanServerObjDeployable(serverObj,machineInfo,useless)){
                globalCloud->AddMachine(i,useless,machineObj);
                Deploy res;
                res.serverID=i;
                res.node=useless;
                receiver.deployList.push_back(res);
                return 0;
            }
        }
        //no existed obj is suitable. have to buy new server
        for(int i=0;i<serverInfoList.size();i++){
            auto serverInfo=serverInfoList[i];
            int useless;
            if(CanServerDeployable(serverInfo,machineInfo,useless)){
                auto serverObj=ServerObj(serverInfo);
                int serverId=globalCloud->AddServerObj(serverObj);

                purchaseMap[std::string(serverInfo.model)]+=1;

                globalCloud->AddMachine(serverId,useless,machineObj);
                Deploy res;
                res.serverID=serverId;
                res.node=useless;
                receiver.deployList.push_back(res);
                return 0;
            }
        }
    }

    return 0;
}

bool SimpleStrategy::CanServerObjDeployable(ServerObj &obj, VirtualMachineInfo &machineInfo, int &deployableNode) {
    auto DoesNodeHaveResource=[](ServerNode node,int requiredCPUNum,int requiredMemSize)->bool {
        return node.remainingCPUNum>requiredCPUNum&&node.remainingMemorySize>requiredMemSize;
    };

    int cpuNum=machineInfo.cpuNum;
    int memorySize=machineInfo.memorySize;
    if(machineInfo.doubleNode==1){
        return DoesNodeHaveResource(obj.nodes[0],cpuNum/2,memorySize/2)
        &&DoesNodeHaveResource(obj.nodes[1],cpuNum/2,memorySize/2);
    }else{
        if(DoesNodeHaveResource(obj.nodes[0],cpuNum,memorySize)){
            deployableNode=0;
            return true;
        }
        else if(DoesNodeHaveResource(obj.nodes[0],cpuNum,memorySize)){
            deployableNode=1;
            return true;
        }else{
            return false;
        }
    }

}

bool SimpleStrategy::CanServerDeployable(ServerInfo &info, VirtualMachineInfo &machineInfo, int &deployableNode) {
    if(machineInfo.doubleNode==1){
        return info.memorySize>machineInfo.memorySize&&info.cpuNum>machineInfo.cpuNum;
    }else{
        if(info.memorySize/2>machineInfo.memorySize&&info.cpuNum/2>machineInfo.cpuNum){
            deployableNode=0;
            return true;
        }
        return false;
    }

}

int SimpleStrategy::HandleDel(Request &del, Result &receiver) {
    int machineId=del.vMachineID;
    globalCloud->DelMachine(machineId);
    return 0;
}

