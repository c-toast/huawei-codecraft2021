//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_VMDEPLOYER_H
#define HUAWEI_CODECRAFT_VMDEPLOYER_H

#include <vector>
#include <map>
#include "vm.h"
#include "server.h"

class VMDeployer{
public:
    std::vector<ServerObj*> DeployerServerList;

    int initWhenNewBatchCome(){return 0;}

    int initWhenNewDayStart(){return 0;}

    int deploy(std::vector<VMObj*> &unhandledVMObj);

    int deployByFitness(std::vector<VMObj *> &unhandledVMObj,int start,int end);

    int forceDeploy(std::vector<VMObj *> &unhandledVMObj);

    int deployByAcceptableUsageState(std::vector<VMObj *> &unhandledVMObj, double acceptableR0);

    int getFakeServerForVM(ServerObj *serverObj, VMObj* vmObj, ServerObj &receiver);

    bool getFakeServerAndJudgeDeployable(ServerObj *serverObj, ServerObj &fakeServerObj, VMObj *vmObj, int &nodeIndex);

    //the index of server that have resource larger than magnitude
    std::vector<std::pair<double,int>> serverIndex; //magnitude,index

    int initServerList();

    int getStartIndexOfServerList(VMObj* vmObj);

};

#endif //HUAWEI_CODECRAFT_VMDEPLOYER_H
