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

//    int deployInServer(int serverNode);
//
//    int deployInCloud(int serverId);

    int getDeployNode();

    std::vector<int> deployNodes;
    VMInfo info;
    int id;
    int deployServerID;

};


#endif //HUAWEI_CODECRAFT_VM_H
