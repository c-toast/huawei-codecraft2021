//
// Created by ctoast on 2021/3/12.
//

#include "dispatcher.h"
#include <algorithm>
#define CANDIDATE_NUM 5

extern RatioCloud* globalCloud;

bool cmpToBuy(ServerInfo& serverA, ServerInfo& serverB)
{
	int costA=0,costB=0,cpu=0,cost=0;
    serverA.getHardwareCost(cost);
    serverA.getCpuNum(cpu);
    costA=cost/cpu;
    serverB.getHardwareCost(cost);
    serverB.getCpuNum(cpu);
    costB=cost/cpu;
    return costA < costB;
}


int Dispatcher::run() {
    std::vector<ServerInfo> serversInfos;
    std::vector<VMInfo> vmachineInfos;
    reader->ReadServersInfo(serversInfos);
    reader->ReadVMachineInfo(vmachineInfos);

    // globalCloud->serverInfoList
    for(auto it:serversInfos){
        int ratio=globalCloud->serverCalculateRatio(it);
        globalCloud->serverInfoList[ratio].push_back(it);
    }
    // globalCloud->serverCandidateList
    for(int ratio=0;ratio<globalCloud->ratioNum;ratio++){
        std::vector<ServerInfo> &it=globalCloud->serverInfoList[ratio];
        std::sort(it.begin(), it.end(), cmpToBuy);
        for(int i=0;i<CANDIDATE_NUM;i++){
            globalCloud->serverCandidateList[ratio].push_back(it[i]);
        }
    }
    // globalCloud->vmInfoMap
    std::map<std::string,VMInfo>& vmInfoMap=globalCloud->vmInfoMap;
    for(auto it:vmachineInfos){
        std::string model;
        it.getModel(model);
        vmInfoMap[model]=it;
    }

    // globalCloud->serverInfoMap
    std::map<std::string,ServerInfo>& serverInfoMap=globalCloud->serverInfoMap;
    for(auto it:serversInfos){
        std::string model;
        it.getModel(model);
        serverInfoMap[model]=it;
    }
    RequestsBunch bunch;
    std::vector<OneDayResult> res;
    reader->ReadBunchOfRequests(bunch);
    strategy->dispatch(bunch,res);
    LOGI("dispatch over.");
    writer->write(res);

    return 0;
}
