//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_STRATEGYTOOLS_H
#define HUAWEI_CODECRAFT_STRATEGYTOOLS_H


#include "server.h"
#include "cmath"
#include "cloud.h"
#include "vmdeployer.h"
#include "cloudoperator.h"
#include "newserverbuyer.h"
#include <array>

extern std::map<std::string,std::vector<std::string>> fitnessRangeMap; //[VMmodel]range
extern std::map<std::string,std::map<std::string,int>> fitnessMap; //[VMmodel][serverModel]fitnessRange


class UsageState{
public:
    static bool isServerNodeInSD(ServerObj* serverObj,int nodeIndex,double R0);

    static bool isServerNodeInAD(ServerObj* serverObj,int nodeIndex,double r0);

    static bool isServerNodeInASD(ServerObj* serverObj,int nodeIndex,double R0,double r0);

    static std::array<double,2> calSingleNodeUsageState(ServerObj* obj, int NodeIndex);
};

class BalanceState{
public:
    static bool isServerBalanceInSD(ServerObj* serverObj,double R0);

    static bool isServerBalanceInAD(ServerObj* serverObj,double r0);

    static bool isServerBalanceInASD(ServerObj* serverObj,double R0,double r0);

    static std::array<double,2> calNodeBalanceState(ServerObj* obj);
};

//double calDeviation(MultiDimension d);

bool isInSD(std::array<double,2> vec, double R0);

bool isInAD(std::array<double,2> vec, double r0);

bool isDeployDecisionBetter(ServerObj *oldServerObj, ServerObj *newServerObj);

double CalculateFullness(ServerObj* serverObj);

int CalFitness(ServerInfo &serverInfo, VMInfo &vmInfo, double &fitnessReceiver);



#endif //HUAWEI_CODECRAFT_STRATEGYTOOLS_H
