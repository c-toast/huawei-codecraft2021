//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_VMDEPLOYER_H
#define HUAWEI_CODECRAFT_VMDEPLOYER_H

#include <vector>
#include <map>
#include "vm.h"

class VMDeployer{
public:
    int initWhenNewBatchCome(){return 0;}

    int initWhenNewDayStart(){return 0;}

    int deploy(std::vector<VMObj*> &unhandledVMObj);

    int forceDeploy(std::vector<VMObj *> &unhandledVMObj);

    int deployByAcceptableUsageState(std::vector<VMObj *> &unhandledVMObj, double acceptableR0);
};

#endif //HUAWEI_CODECRAFT_VMDEPLOYER_H
