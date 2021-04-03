//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "vmdeployer.h"
#include <cloudoperator.h>
#include "global.h"

int Strategy::dispatch(RequestsBatch &batch, std::vector<OneDayResult> &receiver) {
    for(int i=0; i < batch.size(); i++,globalDay++){
        vmMigrater->initWhenNewDayStart();
        vmDeployer->initWhenNewDayStart();
        serverBuyer->initWhenNewDayStart();

        OneDayResult oneDayRes;
        OneDayRequest& oneDayReq=batch[i];

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
        vmMigrater->migrate(unhandledVMObj);
        vmDeployer->deploy(unhandledVMObj);
        serverBuyer->buyAndDeploy(unhandledVMObj);

        cloudOperator.genOneDayOpeRes(unhandledAddReqSet, oneDayRes);
        receiver.push_back(oneDayRes);


        for(auto it:unhandledDelReqSet){
            globalCloud->delVMObjFromCloud(it.vMachineID);
        }
    }
    return 0;
}




