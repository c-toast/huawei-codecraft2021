//
// Created by ctoast on 2021/3/12.
//

#include "dispatcher.h"

extern SimpleCloud* globalCloud;

int Dispatcher::run() {
    std::vector<ServerInfo> serversInfos;
    std::vector<VMInfo> vmachineInfos;
    reader->ReadServersInfo(serversInfos);
    reader->ReadVMachineInfo(vmachineInfos);

    auto& serverInfoMap=globalCloud->serverInfoMap;
    for(auto it:serversInfos){
        std::string model=it.model;
        serverInfoMap[model]=it;
    }

    std::map<std::string,VMInfo>& vmInfoMap=globalCloud->vmInfoMap;
    for(auto it:vmachineInfos){
        std::string model=it.model;
        vmInfoMap[model]=it;
    }

    RequestsBunch bunch;
    std::vector<OneDayResult> res;
    reader->ReadBunchOfRequests(bunch);

    strategy->init();
    strategy->dispatch(bunch,res);

    writer->write(res);

    return 0;
}
