//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_SERVER_H
#define HUAWEI_CODECRAFT_SERVER_H

#include <utility>

#include "vm.h"
#include <map>

class ServerInfo{
public:
    ServerInfo()=default;

    ServerInfo(std::string model,int cpuNum,int memorySize,int hardwareCost,int energyCost):
    model(std::move(model)),cpuNum(cpuNum),memorySize(memorySize),hardwareCost(hardwareCost),energyCost(energyCost){}

    bool canDeployOnSingleNode(VMInfo &vmInfo);

    bool canDeployOnDoubleNode(VMInfo& vmInfo);

    std::string model;
    int cpuNum;
    int memorySize;
    int hardwareCost;
    int energyCost;

    double unitCost=-1;
};

class ServerNode{
public:
    Resource remainingResource;
};

class ServerObj{
public:
    ServerInfo info;
    int id=-1;
    ServerNode nodes[2]{};

    std::map<int,VMObj*> vmObjMap; //[id]vmobj

    std::map<int,int> vmObjDeployNodeMap; //[id]deployNode

    ServerObj()=default;

    ServerObj(ServerInfo serverInfo) : info(std::move(serverInfo)){
        int cpuNumAlloc=info.cpuNum;
        int memorySizeAlloc=info.memorySize;
        cpuNumAlloc/=2;
        memorySizeAlloc/=2;
        Resource r(cpuNumAlloc,memorySizeAlloc);
        nodes[0].remainingResource=r;
        nodes[1].remainingResource=r;
    }

    int getNodeRemainingResource(int nodeIndex,Resource& receiver);

    bool canDeployOnSingleNode(int nodeIndex,VMInfo& vmInfo);

    bool canDeployOnDoubleNode(VMInfo& vmInfo);

    bool canDeployOnNode(int nodeIndex,VMInfo& vmInfo);

    //if vm is single node, the deployNode will be the node that have more residual resource
    bool canDeploy(VMInfo& vmInfo,int& deployNode);

    //nodeIndex can be NODEA, NODEB OR NODEAB
    int deployVM(int nodeIndex, VMObj* vmObj);

    int delVM(int vmID);

    int delVM(int nodeIndex, VMObj* vmObj);

    int allocResForDeploy(int nodeIndex, VMObj* vmObj);

    int freeResForDel(int nodeIndex, VMObj* vmObj);
};




#endif //HUAWEI_CODECRAFT_SERVER_H
