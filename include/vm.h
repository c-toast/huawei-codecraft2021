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

    std::string model;
    int cpuNum;
    int memorySize;
    int doubleNode;
};

class VMObj{
public:
    VMObj()=default;

    VMObj(VMInfo& machineInfo, int machineId): info(machineInfo), id(machineId), deployServerID(-1){}

    //deploy will just record the deployInServer message
    int deployInServer(int serverNode);

    int deployInCloud(int serverId);

    int getInfo(VMInfo& receiver);

    int getID(int& receiver);

    int getDeployServerID(int &receiver);

    std::vector<int> deployNodes;

    VMInfo info;
    int id;
    int deployServerID;
};


#endif //HUAWEI_CODECRAFT_VM_H
