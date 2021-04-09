//
// Created by ctoast on 2021/4/8.
//

#ifndef HUAWEI_CODECRAFT_MONITOR_H
#define HUAWEI_CODECRAFT_MONITOR_H

#include "global.h"

class Monitor : public CloudListener {
public:
    FILE *file;

    Monitor(FILE *file) : file(file) {}

    int deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj) override;

    int moveVMObjFromServerObj(int vmID) override;

    int createServerObj(ServerInfo &serverInfo) override;

    int delVMObjFromCloud(int vmID) override;

    int printVM(VMObj *vmObj);

    int printServer(ServerObj *serverObj);

    int printdeployedVM(VMObj *vmObj);

    std::map<VMObj *, bool> migrateMap;

    int counter = 5000;

};



#endif //HUAWEI_CODECRAFT_MONITOR_H
