//
// Created by ctoast on 2021/3/12.
//

#include "dispatcher.h"
extern SimpleCloud* globalCloud;

int Dispatcher::run() {
    std::vector<ServerInfo> serversInfos;
	std::vector<ServerInfo> serverSorted;
    std::vector<VMInfo> vmachineInfos;
    reader->ReadServersInfo(serversInfos);
    reader->ReadVMachineInfo(vmachineInfos);

    globalCloud->serverInfoList=serversInfos;
	serverSorted = serversInfos;
	std::sort(serverSorted.begin(),serverSorted.end());
	globalCloud->serverSortList = serverSorted;
    std::map<std::string,VMInfo>& vmInfoMap=globalCloud->vmInfoMap;
    for(auto it:vmachineInfos){
        std::string model;
        it.getModel(model);
        vmInfoMap[model]=it;
    }

    RequestsBunch bunch;
    std::vector<OneDayResult> res;
    reader->ReadBunchOfRequests(bunch);
    strategy->dispatch(bunch,res);

    writer->write(res);

    return 0;
}

bool operator <(ServerInfo& server1, ServerInfo& server2) {
	int cpu1, cpu2, mem1, mem2;
	server1.getCpuNum(cpu1);
	server1.getCpuNum(cpu2);
	server1.getMemorySize(mem1);
	server1.getMemorySize(mem2);
	double ratio1 = cpu1 * 1.0 / mem1;
	double ratio2 = cpu2 * 1.0 / mem2;
	return ratio1 < ratio2;
}
