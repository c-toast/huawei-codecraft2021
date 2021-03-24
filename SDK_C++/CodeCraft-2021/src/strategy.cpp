//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "cloud.h"
#include "utils.h"
#include "cstring"
#include "strategytools.h"
#include "newserverbuyer.h"
#include "vmdeployer.h"
#include <cmath>
#include <algorithm>
#include <cloudoperator.h>

int Strategy::dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) {
    int dayNum=requestsBunch.dayNum;
    auto& bunch=requestsBunch.bunch;

    nsd.learnModelInfo();

    for(int i=0;i<dayNum;i++){
        OneDayResult oneDayRes;
        OneDayRequest& oneDayReq=bunch[i];

        std::vector<VMObj *> unhandledVMObj;
        std::vector<Request> unhandledDelReqSet;
        std::vector<Request> unhandledAddReqSet;

        osd.deployVMNum=globalCloud->vmObjMap.size();

        for(auto it:oneDayReq){
            if(it.op==ADD){
                auto vmObj=globalCloud->createVMObj(it.vMachineID,it.vMachineModel);
                unhandledVMObj.push_back(vmObj);
                unhandledAddReqSet.push_back(it);
            }else{
                unhandledDelReqSet.push_back(it);
            }
        }
        osd.migrateAndDeploy(unhandledVMObj);
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
    globalCloud->delVMObjFromCloud(machineId);
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

