//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "vmdeployer.h"
#include <cloudoperator.h>
#include "global.h"

int Strategy::dispatch(RequestsBatch &batch) {
    std::vector<OneDayResult> res;

    for(int i=0; i < batch.size(); i++,globalDay++){
        OneDayResult oneDayRes;
        OneDayRequest& oneDayReq=batch[i];

        //initialize Info!!!
        vmMigrater->initWhenNewDayStart(oneDayReq);
        vmDeployer->initWhenNewDayStart();
        serverBuyer->initWhenNewDayStart(oneDayReq);
        cloudOperator.initWhenNewDayStart(oneDayReq);


        std::vector<VMObj *> unhandledVMObj;
        std::vector<Request> unhandledDelReqSet;
        std::vector<Request> unhandledAddReqSet;

        for(auto it:oneDayReq){
            if(it.op==ADD){
                VMObj* vmObj=globalCloud->vmObjMap[it.vmID];
                unhandledVMObj.push_back(vmObj);
                unhandledAddReqSet.push_back(it);
            }else{
                unhandledDelReqSet.push_back(it);
            }
        }
        vmMigrater->migrate(unhandledVMObj);
        cloudOperator.depTree.init(cloudOperator.migrationVec);
        vmDeployer->deploy(unhandledVMObj);
        serverBuyer->buyAndDeploy(unhandledVMObj);

        for(auto it:unhandledDelReqSet){
            cloudOperator.delVMObjFromCloud(it.vmID);
        }

        cloudOperator.genOneDayOpeRes(unhandledAddReqSet, oneDayRes);
        res.push_back(oneDayRes);
    }

    writer.write(res);
    fflush(stdout);
    return 0;
}




