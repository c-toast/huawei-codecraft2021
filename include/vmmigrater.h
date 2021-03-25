//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_VMMIGRATER_H
#define HUAWEI_CODECRAFT_VMMIGRATER_H

#include "vm.h"
#include "strategytools.h"

class VMMigrater {
public:
    int deployVMNum=0;

    int migrate(std::vector<VMObj *> &unhandledVMObj);

    int migrateByUsageState(std::vector<VMObj *> &unhandledVMObj);

    int migrateByNodeBalance(std::vector<VMObj *> &unhandledVMObj);
};


#endif //HUAWEI_CODECRAFT_VMMIGRATER_H
