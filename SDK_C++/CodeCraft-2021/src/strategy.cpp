//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "vmdeployer.h"
#include <cloudoperator.h>
#include "global.h"

int Strategy::dispatch(RequestsBatch &requestsBatch, std::vector<OneDayResult> &receiver) {
    int dayNum=requestsBatch.size();

    for(int i=0;i<dayNum;i++,globalDay++){
        OneDayResult oneDayRes;
        OneDayRequest& oneDayReq=requestsBatch[i];

        std::vector<VMObj *> unhandledVMObj;
        std::vector<Request> unhandledDelReqSet;
        std::vector<Request> unhandledAddReqSet;

        vmMigrater->availableMigrateTime= (globalCloud->vmObjMap.size() * 5) / 1000;

        for(auto it:oneDayReq){
            if(it.op==ADD){
                auto vmObj=globalCloud->createVMObj(it.vMachineID,it.vMachineModel);
                unhandledVMObj.push_back(vmObj);
                unhandledAddReqSet.push_back(it);
            }else{
                unhandledDelReqSet.push_back(it);
            }
        }
        vmMigrater->migrate(unhandledVMObj);
        vmDeployer->deploy(unhandledVMObj);
        serverBuyer->buyAndDeploy(unhandledVMObj);

        cloudOperator.genOneDayOpeRes(unhandledAddReqSet, oneDayRes);
        receiver.push_back(oneDayRes);

        for(auto it:unhandledDelReqSet){
            HandleDel(it,oneDayRes);
        }
    }
    return 0;
}

int Strategy::HandleDel(Request &del, OneDayResult &receiver) {
    int machineId=del.vMachineID;
    globalCloud->delVMObjFromCloud(machineId);
    return 0;
}



