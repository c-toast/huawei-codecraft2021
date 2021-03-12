//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_STRATEGY_H
#define HUAWEI_CODECRAFT_STRATEGY_H

#include <map>
#include <string>
#include "readwriter.h"

class Strategy{
public:
    virtual int dispatch(RequestsBunch &requestsBunch, std::vector<Result> &receiver) =0;
};

class SimpleStrategy: public Strategy{
    std::map<std::string,int> purchaseMap;

    int dispatch(RequestsBunch &requestsBunch, std::vector<Result> &receiver) override;

    int HandleAdd(Request &req, Result &receiver);

    int HandleDel(Request &del, Result &receiver);

    bool CanServerObjDeployable(ServerObj &obj, VirtualMachineInfo &machineInfo, int &deployableNode);

    bool CanServerDeployable(ServerInfo &info, VirtualMachineInfo &machineInfo, int &deployableNode);
};



#endif //HUAWEI_CODECRAFT_STRATEGY_H
