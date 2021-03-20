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
    virtual int init(){};

    virtual int dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) =0;
};

class MigratedCandidate{

};

class SimpleStrategy: public Strategy{
    int init() override;

    std::map<std::string,std::vector<std::string>> fitnessRangeMap; //[VMmodel]range

    std::map<std::string,std::map<std::string,int>> fitnessMap; //[VMmodel][serverModel]fitnessRange

    std::vector<std::string> purchaseVec;


    int dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) override;

    int HandleAdd(Request &req, OneDayResult &receiver);

    int HandleDel(Request &del, OneDayResult &receiver);

    bool CanServerObjDeployable(ServerObj &obj, VMInfo &machineInfo, int &deployableNode);

    bool CanServerDeployable(ServerInfo &info, VMInfo &machineInfo, int &deployableNode);

    int AddVMInOldServer(std::vector<Request> &unhandledAddReqSet, OneDayResult &receiver);

    int AddVMInNewServer(std::vector<Request> &unhandledAddReqSet, OneDayResult &receiver);

    int CalFitness(ServerInfo &serverInfo, VMInfo &vmInfo, double &fitnessReceiver);
};



#endif //HUAWEI_CODECRAFT_STRATEGY_H
