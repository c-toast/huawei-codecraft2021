//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_VIRTUAL_MACHINE_H
#define HUAWEI_CODECRAFT_VIRTUAL_MACHINE_H

#include "vector"

class VirtualMachineInfo{
public:
    char model[21];
    int cpuNum;
    int memorySize;
    int doubleNode;
};

class VirtualMachineObj{
public:
    VirtualMachineObj()=delete;

    VirtualMachineObj(VirtualMachineInfo machineInfo,int machineId){info=machineInfo;ID=machineId;}

    VirtualMachineInfo info;
    int ID;
    int deployedServerID;
    std::vector<int> deployedNodes;
};


#endif //HUAWEI_CODECRAFT_VIRTUAL_MACHINE_H
