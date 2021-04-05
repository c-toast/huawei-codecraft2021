//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "vmdeployer.h"
#include <cloudoperator.h>
#include "global.h"

int Strategy::dispatch(RequestsBatch &batch, std::vector<OneDayResult> &receiver) {
    for(int i=0; i < batch.size(); i++,globalDay++){
        OneDayResult oneDayRes;
        OneDayRequest& oneDayReq=batch[i];

        vmMigrater->initWhenNewDayStart(oneDayReq);
        vmDeployer->initWhenNewDayStart();
        serverBuyer->initWhenNewDayStart();


        std::vector<VMObj *> unhandledVMObj;
        std::vector<Request> unhandledDelReqSet;
        std::vector<Request> unhandledAllDelReqSet;
        std::vector<Request> unhandledAddReqSet;

        std::vector<VMObj *> unhandledMigrateVMObj;
        vmMigrater->migrate(unhandledMigrateVMObj);
        vmDeployer->deploy(unhandledMigrateVMObj);
        for(auto it:oneDayReq){
            if(it.op==ADD){
                auto vmObj=globalCloud->createVMObj(it.vMachineID,it.vMachineModel);
                unhandledVMObj.push_back(vmObj);
                unhandledAddReqSet.push_back(it);
                cloudOperator.newVMMap[vmObj]=true;
            }else{
                unhandledDelReqSet.push_back(it);
                unhandledAllDelReqSet.push_back(it);
            }

            if(unhandledDelReqSet.size()>20){
                vmDeployer->deploy(unhandledVMObj);
                serverBuyer->buyAndDeploy(unhandledVMObj);

                for(auto it:unhandledDelReqSet){
                    cloudOperator.delVMObjFromCloud(it.vMachineID);
                }
                unhandledDelReqSet.clear();
                unhandledVMObj.clear();
            }
        }

        vmDeployer->deploy(unhandledVMObj);
        unhandledVMObj.insert(unhandledVMObj.end(),unhandledMigrateVMObj.begin(),unhandledMigrateVMObj.end());
        serverBuyer->buyAndDeploy(unhandledVMObj);

        for(auto it:unhandledDelReqSet){
            cloudOperator.delVMObjFromCloud(it.vMachineID);
        }

        cloudOperator.genOneDayOpeRes(unhandledAddReqSet, oneDayRes);
        receiver.push_back(oneDayRes);

        for(auto it:unhandledAllDelReqSet){
            globalCloud->eraseVMObj(it.vMachineID);
        }


    }
    return 0;
}




