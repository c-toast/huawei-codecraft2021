//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_STRATEGY_H
#define HUAWEI_CODECRAFT_STRATEGY_H

#include <map>
#include <string>
#include "readwriter.h"
#include <cmath>

class NewServerDeployer{
public:
    std::map<double,std::vector<ServerInfo*>> Clusters; //[radio][servercluster]

    int learnModelInfo();

    int learnPosteriorInfo(RequestsBunch &requestsBunch);

    int buyAndDeploy(std::vector<VMObj*> &unhandledVMObj);

    int buyAndDeployDoubleNode(std::vector<VMObj*> &unhandledVMObj);

    int buyAndDeploySingleNode(std::vector<VMObj*> &singleNodeVMObj);

    int classify(std::vector<VMObj *> &vmObjVec,std::map<double,std::vector<VMObj*>>& receiver);

    int movVMObjToNewServerObj(ServerObj *oldObj, ServerObj *newObj);
};

class OldServerDeployer{
public:
    int deployVMNum=0;

    int migrateAndDeploy(std::vector<VMObj*> &unhandledVMObj);

    int migrate(std::vector<VMObj*> &unhandledVMObj);

    int Deploy(std::vector<VMObj*> &unhandledVMObj);
};

class ResultRecorder{
public:
    int oldSize=0;

    std::map<VMObj*,bool> haveObjDeploy;

    std::vector<VMObj*> migrationVec;

    int movVMObj();

    int deployVMObj(int serverObjID, int nodeIndex, int vmID);

    int deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj);

    int BuyServer();

    int ouputOneDayRes(std::vector<Request>,OneDayResult &receiver);

    int migrateVMObj(ServerObj* serverObj,VMObj* vmObj);
};

class Strategy{
public:
    int init();

    int dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver);

    int HandleDel(Request &del, OneDayResult &receiver);

    int CalFitness(ServerInfo &serverInfo, VMInfo &vmInfo, double &fitnessReceiver);
};

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

bool isDeployDecisionBetter(ServerObj *oldServerObj, ServerObj *newServerObj);

bool isMigrateDecisionBetter(ServerObj *oldServerObj, ServerObj *newServerObj);

double CalculateFullness(ServerObj* serverObj);
#endif //HUAWEI_CODECRAFT_STRATEGY_H
