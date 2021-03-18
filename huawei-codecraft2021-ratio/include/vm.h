//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_VM_H
#define HUAWEI_CODECRAFT_VM_H

#include <string>
#include <utility>
#include "vector"
#include "utils.h"

class VMInfo{
public:
    VMInfo()=default;

    VMInfo(std::string model,int cpuNum,int memorySize,int doubleNode):
    model(std::move(model)),cpuNum(cpuNum),memorySize(memorySize),doubleNode(doubleNode){;};

    int getModel(std::string& receiver);

    int getCpuNum(int& receiver);

    int getMemorySize(int& receiver);

    int getDoubleNode(int &receiver);

    int getRequiredResourceForOneNode(Resource& receiver) const;

private:
    std::string model;
    // cpuNum and memorySize is for both nodes of VM.
    int cpuNum;
    int memorySize;
    int doubleNode;
};

class VMObj{
public:
    VMObj()=default;

    VMObj(VMInfo& machineInfo, int machineId): info(machineInfo), id(machineId), deployServerID(-1), deployRatioID(-1){}

    //deploy will just record the deploy message
    int deploy(int serverId, int serverNode, int ratioID);

    int getInfo(VMInfo& receiver);

    int getID(int& receiver);

    int getDeployServerID(int &receiver);

    int getDeployRatioID(int &receiver);

    int getDeployNode(int &receiver);

    std::vector<int> deployNodes;

private:
    VMInfo info;
    int id;
    int deployServerID;
    int deployRatioID;
};


#endif //HUAWEI_CODECRAFT_VM_H
