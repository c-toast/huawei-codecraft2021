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

    int sortServerVMObj(ServerObj* serverObj, std::vector<VMObj *> &receiver);

    int initWhenNewBatchCome(){return 0;}

    int initWhenNewDayStart(OneDayRequest &oneDayReq);

};


#endif //HUAWEI_CODECRAFT_VMMIGRATER_H
