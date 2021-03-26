//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_VMDEPLOYER_H
#define HUAWEI_CODECRAFT_VMDEPLOYER_H

#include <vector>
#include <map>
#include "vm.h"

struct DoubleNodeVMWrapper{
    VMObj* vm1;
    VMObj* vm2;
    bool isPair;
    DoubleNodeVMWrapper(VMObj* vm1,VMObj* vm2,bool isPair): vm1(vm1), vm2(vm2), isPair(isPair){};
};

int separateUnhandledVM(std::vector<VMObj *> &unhandledSingleVMObj,std::vector<DoubleNodeVMWrapper>& unhandledDoubleNodeVMObj);

int aggregateUnhandledVM(std::vector<VMObj *> &unhandledSingleVMObj,std::vector<DoubleNodeVMWrapper>& unhandledDoubleNodeVMObj);

bool isSimiliar(VMInfo& info1,VMInfo& info2);

class VMDeployer{
public:
    int deploy(std::vector<VMObj*> &unhandledVMObj);

    int deployDoubleNodeVM(std::vector<DoubleNodeVMWrapper>& wp1);

    int deploySingleNodeVM(std::vector<VMObj *> &unhandledSingleVMObj);

    int forceDeploy(std::vector<VMObj *> &unhandledVMObj);

    int deployByAcceptableUsageState(std::vector<VMObj *> &unhandledVMObj, double acceptableR0);

    int forceDeploy(std::vector<DoubleNodeVMWrapper>& unhandledDoubleVMObj);

    int deployByAcceptableUsageState(std::vector<DoubleNodeVMWrapper>& unhandledDoubleVMObj, double acceptableR0);
};

#endif //HUAWEI_CODECRAFT_VMDEPLOYER_H
