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

extern std::map<std::string,std::vector<std::string>> fitnessRangeMap; //[VMmodel]range
extern std::map<std::string,std::map<std::string,int>> fitnessMap; //[VMmodel][serverModel]fitnessRange
#define ACCEPT_RANGE 5

#define USAGESTATERO 1
#define USAGESTATErO 0.3
#define NODEBANLANCESTATERO 1

struct MultiDimension{
    double Dimension1;
    double Dimension2;
};

MultiDimension calSingleNodeUsageState(ServerObj* obj, int NodeIndex);

MultiDimension calNodeBalanceState(ServerObj* obj);

inline double distance(double x1,double y1,double x2,double y2){
    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

double calDeviation(MultiDimension d);

bool isInSD(MultiDimension us, double R0);

bool isServerInSD(ServerObj* serverObj, double R0);

bool isServerNodeBalance(ServerObj* serverObj,double R0);

bool isDeployDecisionBetter(ServerObj *oldServerObj, ServerObj *newServerObj);

bool isMigrateDecisionBetter(ServerObj *oldServerObj, ServerObj *newServerObj);

double CalculateFullness(ServerObj* serverObj);

int CalFitness(ServerInfo &serverInfo, VMInfo &vmInfo, double &fitnessReceiver);



#endif //HUAWEI_CODECRAFT_STRATEGYTOOLS_H
