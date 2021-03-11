//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_VIRTUAL_MACHINE_H
#define HUAWEI_CODECRAFT_VIRTUAL_MACHINE_H

class VirtualMachineInfo{
public:
    char model[21];
    int cpuNum;
    int memorySize;
    int doubleNode;
};

class VirtualMachineObj{
public:
    VirtualMachineInfo info;
    int ID;
    int deployedServerID;
};


#endif //HUAWEI_CODECRAFT_VIRTUAL_MACHINE_H
