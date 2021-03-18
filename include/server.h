//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_SERVER_H
#define HUAWEI_CODECRAFT_SERVER_H

#include <utility>

#include "vm.h"

class ServerInfo{
public:
    ServerInfo()=default;

    ServerInfo(std::string model,int cpuNum,int memorySize,int hardwareCost,int energyCost):
    model(std::move(model)),cpuNum(cpuNum),memorySize(memorySize),hardwareCost(hardwareCost),energyCost(energyCost){}

    int getModel(std::string& receiver);

    int getCpuNum(int& receiver);

    int getMemorySize(int& receiver);

    int getHardwareCost(int& receiver);

    int getEnergyCost(int& receiver);

    bool canDeployOnSingleNode(VMInfo &vmInfo);

    bool canDeployOnDoubleNode(VMInfo& vmInfo);


private:
    std::string model;
    int cpuNum;
    int memorySize;
    int hardwareCost;
    int energyCost;
};

class ServerNode{
public:
    Resource remainingResource;
};

class ServerObj{
public:
    ServerInfo info;
    int ID;
    int ratioID;
    ServerNode nodes[2]{};

    ServerObj()=default;

    ServerObj(ServerInfo serverInfo,int serverID,int RatioID): info(std::move(serverInfo)),ID(serverID),ratioID(RatioID){
        int cpuNumAlloc;info.getCpuNum(cpuNumAlloc);
        cpuNumAlloc/=2;
        int memorySizeAlloc;info.getMemorySize(memorySizeAlloc);
        memorySizeAlloc/=2;

        Resource r(cpuNumAlloc,memorySizeAlloc);
        nodes[0].remainingResource=r;
        nodes[1].remainingResource=r;
    }

    int getModel(std::string& receiver);

    int getNodeRemainingResource(int nodeIndex,Resource& receiver);

    bool canDeployOnSingleNode(int nodeIndex,VMInfo& vmInfo);

    bool canDeployOnDoubleNode(VMInfo& vmInfo);

    //just deploy in one node. if the VM type is DOUBLE, you need to call deployVM twice with different nodeIndex
    int deployVM(int nodeIndex, VMObj &receiver);

    //same as deployVM
    int delVM(int nodeIndex, VMInfo &vmInfo);
};




#endif //HUAWEI_CODECRAFT_SERVER_H
