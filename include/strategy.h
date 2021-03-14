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
    virtual int dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) =0;
};

class SimpleStrategy: public Strategy{
    std::map<std::string,int> purchaseMap;

    int dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) override;

    int HandleAdd(Request &req, OneDayResult &receiver);

    int HandleDel(Request &del, OneDayResult &receiver);

    bool CanServerObjDeployable(ServerObj &obj, VMInfo &machineInfo, int &deployableNode);

    bool CanServerDeployable(ServerInfo &info, VMInfo &machineInfo, int &deployableNode);
};



#endif //HUAWEI_CODECRAFT_STRATEGY_H
