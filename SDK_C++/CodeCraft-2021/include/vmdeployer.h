//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_VMDEPLOYER_H
#define HUAWEI_CODECRAFT_VMDEPLOYER_H

#include <vector>
#include "vm.h"

class VMDeployer{
public:
    int deploy(std::vector<VMObj*> &unhandledVMObj);

    int deployByAcceptableUsageState(std::vector<VMObj *> &unhandledVMObj, double acceptableR0);

    int deployDoubleNodeVM();

    int deploySingleNodeVM();

    int forceDeploy(std::vector<VMObj *> &unhandledVMObj);
};

#endif //HUAWEI_CODECRAFT_VMDEPLOYER_H
