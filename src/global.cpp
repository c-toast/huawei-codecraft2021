//
// Created by ctoast on 2021/3/24.
//

#include <algorithm>
#include "global.h"

Cloud *globalCloud = new Cloud();
CloudOperator cloudOperator;

int globalDay = 0;
int totalDayNum, readableDaylimit;
RequestsBatch futureRequestsBatch;

StdReader reader;
StdWriter writer;

int initCloud() {
    std::vector<ServerInfo> serversInfos;
    std::vector<VMInfo> vmInfos;
    reader.ReadServersInfo(serversInfos);
    reader.ReadVMachineInfo(vmInfos);

    auto &serverInfoMap = globalCloud->serverInfoMap;
    for (auto it:serversInfos) {
        std::string model = it.model;
        serverInfoMap[model] = it;
    }

    std::map<std::string, VMInfo> &vmInfoMap = globalCloud->vmInfoMap;
    for (auto it:vmInfos) {
        std::string model = it.model;
        vmInfoMap[model] = it;
    }

    return 0;
}


int initFitness() {
    auto &serverInfoMap = globalCloud->serverInfoMap;
    auto &vmInfoMap = globalCloud->vmInfoMap;
    struct FitRecord {
        std::string serverModel;
        double fitness;
    };

    auto reqSetCmp = [](const FitRecord &r1, const FitRecord &r2) {
        return r1.fitness < r2.fitness;
    };

    for (auto &vmInfoMapIt:vmInfoMap) {
        std::vector<FitRecord> fitVec;
        for (auto &serverInfoListIt:serverInfoMap) {
            FitRecord r;
            r.serverModel = serverInfoListIt.second.model;
            r.fitness = CalFitness(serverInfoListIt.second, vmInfoMapIt.second);
//            FLOGI("server %d %d vm %d %d fitness %f", serverInfoListIt.second.cpuNum,
//                  serverInfoListIt.second.memorySize, vmInfoMapIt.second.cpuNum, vmInfoMapIt.second.memorySize,
//                  r.fitness);

            fitVec.push_back(r);
        }
        std::sort(fitVec.begin(), fitVec.end(), reqSetCmp);
        int i = 0;
        for (auto &recordIt:fitVec) {
            serversSortByFitness[vmInfoMapIt.first].push_back(recordIt.serverModel);
            fitnessRangeMap[vmInfoMapIt.first][recordIt.serverModel] = i;
            fitnessMap[vmInfoMapIt.first][recordIt.serverModel] = recordIt.fitness;
            i++;
        }
    }

    return 0;
}

int initAll() {
    initCloud();
    initFitness();
    return 0;
}




