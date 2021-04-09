//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_VMMIGRATER_H
#define HUAWEI_CODECRAFT_VMMIGRATER_H

#include "vm.h"
#include "strategytools.h"

class migraterListener : public CloudListener {
public:
    std::map<int,VMObj*> candidateVMMap;

    int deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj) override;

    int moveVMObjFromServerObj(int vmID) override;

};

class VMMigrater {
public:
    int availableMigrateTime=0;

    int migrate(std::vector<VMObj *> &unhandledVMObj);

    int migrateByUsageState(std::vector<VMObj *> &unhandledVMObj, ServerObj* simulatedServerObj);

    int migrateByFitness(std::vector<VMObj *> &unhandledVMObj);

    int migrateByNodeBalance(std::vector<VMObj *> &unhandledVMObj, ServerObj* simulatedServerObj);

    int sortServerVMObj(ServerObj* serverObj, std::vector<VMObj *> &receiver);

    int initWhenNewBatchCome(){return 0;}

    int initWhenNewDayStart(OneDayRequest &oneDayReq);

    int init();

    migraterListener listener;
};


#endif //HUAWEI_CODECRAFT_VMMIGRATER_H
