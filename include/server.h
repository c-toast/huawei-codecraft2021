//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_SERVER_H
#define HUAWEI_CODECRAFT_SERVER_H

class ServerInfo{
public:
    char model[21];
    int cpuNum;
    int memorySize;
    int hardwareCost;
    int energyCost;
};

class ServerObj{
    ServerInfo info;
    int ID;
    int remainingCPUNum;
    int remainingMemorySize;
};


#endif //HUAWEI_CODECRAFT_SERVER_H
