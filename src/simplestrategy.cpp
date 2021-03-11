//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "cloud.h"
#include "utils.h"

extern SimpleCloud* globalCloud;

int SimpleStrategy::dispatch(RequestsBunch &requestsBunch, Result &receiver) {
    int dayNum=requestsBunch.dayNum;
    auto& bunch=requestsBunch.bunch;
    for(int i=0;i<dayNum;i++){
        OneDayRequest& oneDayReq=bunch[i];
        for(auto it:oneDayReq){
            if(it.op==ADD){
                HandleAdd(it,receiver);
            }else{
                HandleDel(it,receiver);
            }
        }
    }
    return 0;
}

int SimpleStrategy::HandleAdd(Request &req, Result &receiver) {
    auto& serverInfoList=globalCloud->serverInfoList;
    auto& vMachineInfoMap= globalCloud->vMachineInfoMap;
    auto& serverObjList=globalCloud->serverObjList;
    auto id=req.vMachineID;
    auto model=req.vMachineModel;

    std::string model_str(model);
    auto InfoIt=vMachineInfoMap.find(model_str);
    if(InfoIt==vMachineInfoMap.end()){
        LOGE("machine model does not exist");
        exit(-1);
    }

    VirtualMachineInfo machineInfo= InfoIt->second;

    for(auto serverObj:serverObjList){
        if(serverObj.remainingMemorySize<machineInfo.memorySize
        &&serverObj.remainingCPUNum<machineInfo.cpuNum){
            
        }
    }

    return 0;
}

