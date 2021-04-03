//
// Created by ctoast on 2021/3/24.
//

#include <algorithm>
#include "global.h"

Cloud* globalCloud=new Cloud();
CloudOperator cloudOperator;

int totalDay=0;
int globalDay=0;
RequestsBatch requestsBatch;

StdReader reader;
StdWriter writer;

int initCloud() {
    std::vector<ServerInfo> serversInfos;
    std::vector<VMInfo> vmInfos;
    reader.ReadServersInfo(serversInfos);
    reader.ReadVMachineInfo(vmInfos);

    auto& serverInfoMap=globalCloud->serverInfoMap;
    for(auto it:serversInfos){
        std::string model=it.model;
        serverInfoMap[model]=it;
    }

    std::map<std::string,VMInfo>& vmInfoMap=globalCloud->vmInfoMap;
    for(auto it:vmInfos){
        std::string model=it.model;
        vmInfoMap[model]=it;
    }

    return 0;
}



int initFitness() {
    auto& serverInfoMap= globalCloud->serverInfoMap;
    auto& vmInfoMap=globalCloud->vmInfoMap;
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

int initAll(){
    initCloud();
    initFitness();
    return 0;
}




