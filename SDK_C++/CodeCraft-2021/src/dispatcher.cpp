//
// Created by ctoast on 2021/3/12.
//

#include "dispatcher.h"

extern SimpleCloud* globalCloud;

int Dispatcher::run() {
    std::vector<ServerInfo> serversInfos;
    std::vector<VirtualMachineInfo> vmachineInfos;
    reader->ReadServersInfo(serversInfos);
    reader->ReadVMachineInfo(vmachineInfos);

    globalCloud->serverInfoList=serversInfos;
    std::map<std::string,VirtualMachineInfo>& vmInfoMap=globalCloud->vMachineInfoMap;
    for(auto it:vmachineInfos){
        vmInfoMap[std::string(it.model)]=it;
    }

    RequestsBunch bunch;
    std::vector<Result> res;
    reader->ReadBunchOfRequests(bunch);
    strategy->dispatch(bunch,res);

    writer->write(res);

    return 0;
}
