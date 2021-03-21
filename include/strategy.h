//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_STRATEGY_H
#define HUAWEI_CODECRAFT_STRATEGY_H

#include <map>
#include <string>
#include "readwriter.h"

class NewServerDeployer{
public:
    int learnModelInfo();

    int learnPosteriorInfo(RequestsBunch &requestsBunch);

    int buyAndDeploy(std::vector<VMObj*> &unhandledVMObj);
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



#endif //HUAWEI_CODECRAFT_STRATEGY_H
