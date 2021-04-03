#include <iostream>
#include "dispatcher.h"
#include "global.h"
#include "cloud.h"

class Monitor : public CloudListener {
public:
    FILE *file;

    Monitor(FILE *file) : file(file) {}

    int deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj);


    int delVMObjFromServerObj(int vmID);

    int printVM(VMObj *vmObj);

    int printServer(ServerObj *serverObj);

    std::map<VMObj *, bool> migrateMap;

};

int Monitor::delVMObjFromServerObj(int vmID) {
    VMObj *vmObj = globalCloud->vmObjMap[vmID];
    fprintf(file, "migrate VM, vmInfo:");
    printVM(vmObj);
    fprintf(file, "source server:");
    printServer(globalCloud->serverObjList[vmObj->id]);
    migrateMap[vmObj] = true;
}

int Monitor::deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj) {
    if (migrateMap.find(vmObj) != migrateMap.end()) {
        migrateMap.erase(vmObj);
        fprintf(file, "target server:");
        printServer(globalCloud->serverObjList[serverObjID]);
    } else {
        fprintf(file, "deploy:");
        printServer(globalCloud->serverObjList[serverObjID]);
    }
}

int Monitor::printVM(VMObj *vmObj) {
    int deployedNode = vmObj->info.doubleNode == 1 ? NODEAB : vmObj->deployNodes[0];
    fprintf(file, "vmID %d, deployNode %d, requiredres %d %d\n", vmObj->id, deployedNode,
            vmObj->info.cpuNum, vmObj->info.memorySize);
    return 0;
}

int Monitor::printServer(ServerObj *serverObj) {
    fprintf(file, "serverID %d, ownRes %d %d\nnodeA remainingRes %d %d, nodeB remainingRes %d %d\n", serverObj->id,
            serverObj->info.cpuNum, serverObj->info.memorySize,
            serverObj->nodes[0].remainingResource.cpuNum, serverObj->nodes[0].remainingResource.memorySize,
            serverObj->nodes[1].remainingResource.cpuNum, serverObj->nodes[1].remainingResource.memorySize);
    fprintf(file, "deployedVM info:\n");
    for (auto it:serverObj->vmObjMap) {
        printVM(it.second);
    }
    return 0;
}


int main() {
//    reader.file=fopen("/home/ctoast/huawei-codecraft/bin/training-1.txt","r");
//    writer.file=fopen("/home/ctoast/huawei-codecraft/bin/result.txt","w");

    initAll();
    VMDeployer defaultVMDeployer;
    NewServerBuyer defaultServerBuyer;
    VMMigrater defaultVMMigrater;

    Strategy strategy(&defaultVMDeployer, &defaultServerBuyer, &defaultVMMigrater);
    Dispatcher dispatcher(&strategy);
    dispatcher.run();

    return 0;
}
