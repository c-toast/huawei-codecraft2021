//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_VMMIGRATER_H
#define HUAWEI_CODECRAFT_VMMIGRATER_H

#include "vm.h"
#include "strategytools.h"

class VMMigrater {
public:
    int availableMigrateTime=0;

    int migrate(std::vector<VMObj *> &unhandledVMObj);

    int migrateByUsageState(std::vector<VMObj *> &unhandledVMObj, ServerObj* simulatedServerObj);

    int migrateByNodeBalance(std::vector<VMObj *> &unhandledVMObj, ServerObj* simulatedServerObj);

    int migrateByVMNum(std::vector<VMObj *> &unhandledVMObj, ServerObj* simulatedServerObj);

};


#endif //HUAWEI_CODECRAFT_VMMIGRATER_H
