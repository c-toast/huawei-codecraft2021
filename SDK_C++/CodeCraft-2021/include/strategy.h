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
    int Deploy(std::vector<VMObj*> &unhandledVMObj);
};

class ResultRecorder{
public:
    int oldSize=0;

    //int deployVMObj(int serverObjID, int nodeIndex, int vmID);

    int deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj);

    int BuyServer();

    int ouputOneDayRes(std::vector<Request>,OneDayResult &receiver);
};

class Strategy{
public:
    int init();

    int dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver);

    int HandleAdd(Request &req, OneDayResult &receiver);

    int HandleDel(Request &del, OneDayResult &receiver);

    bool CanServerObjDeployable(ServerObj &obj, VMInfo &machineInfo, int &deployableNode);

    bool CanServerDeployable(ServerInfo &info, VMInfo &machineInfo, int &deployableNode);

    int AddVMInOldServer(std::vector<Request> &unhandledAddReqSet, OneDayResult &receiver);

    int AddVMInNewServer(std::vector<Request> &unhandledAddReqSet, OneDayResult &receiver);

    int CalFitness(ServerInfo &serverInfo, VMInfo &vmInfo, double &fitnessReceiver);
};



#endif //HUAWEI_CODECRAFT_STRATEGY_H
