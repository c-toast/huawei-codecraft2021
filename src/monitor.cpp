//
// Created by ctoast on 2021/4/8.
//

#include "monitor.h"


int Monitor::moveVMObjFromServerObj(int vmID) {
    if (counter < 0) {
        return 0;
    }
    counter--;

    VMObj *vmObj = globalCloud->vmObjMap[vmID];
    fprintf(file, "\nmigrate VM, vmInfo:\n");
    printdeployedVM(vmObj);
    fprintf(file, "source server:\n");
    printServer(globalCloud->serverObjList[vmObj->deployServerID]);
    migrateMap[vmObj] = true;
    return 0;
}

int Monitor::deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj) {
    if (counter < 0) {
        return 0;
    }
    counter--;

    if (migrateMap.find(vmObj) != migrateMap.end()) {
        migrateMap.erase(vmObj);
        fprintf(file, "\nmigrate:\n");
        printVM(vmObj);
        fprintf(file, "migrate node %d, target server:\n", nodeIndex);
        printServer(globalCloud->serverObjList[serverObjID]);
    } else {
        fprintf(file, "\ndeploy:\n");
        printVM(vmObj);
        printServer(globalCloud->serverObjList[serverObjID]);
    }
    return 0;
}

int Monitor::printVM(VMObj *vmObj) {
    fprintf(file, "vmID %d, doublenode %d, requiredres %d %d\n", vmObj->id, vmObj->info.doubleNode,
            vmObj->info.cpuNum, vmObj->info.memorySize);
    return 0;
}

int Monitor::printdeployedVM(VMObj *vmObj) {
    int deployNode = vmObj->info.doubleNode == 1 ? NODEAB : vmObj->deployNodes[0];
    fprintf(file, "vmID %d, deployednode %d, requiredres %d %d\n", vmObj->id, deployNode,
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
        printdeployedVM(it.second);
    }
    return 0;
}

int Monitor::createServerObj(ServerInfo &serverInfo) {
    if (counter < 0) {
        return 0;
    }
    counter--;

    fprintf(file, "\ncreate new server\n");
    fprintf(file, "model %s, ownRes %d %d\n", serverInfo.model.c_str(), serverInfo.cpuNum, serverInfo.memorySize);
    return 0;
}

int Monitor::delVMObjFromCloud(int vmID) {
    if (counter < 0) {
        return 0;
    }
    counter--;

    VMObj *vmObj = globalCloud->vmObjMap[vmID];
    fprintf(file, "\ndel VM, vmInfo:\n");
    printdeployedVM(vmObj);
    return 0;
}