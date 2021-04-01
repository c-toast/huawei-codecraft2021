//
// Created by ctoast on 2021/3/12.
//

#include "readwriter.h"
#include "cstring"
#include <iostream>


int StdReader::ReadServersInfo(std::vector<ServerInfo> &receiver) {
    int num;
    fscanf(file, "%d", &num);
    char model[21];
    int cpuNum, memorySize, hardwareCost, energyCost;

    fgetc(file);
    for (int i = 0; i < num; i++) {
        fgetc(file);
        for (int j = 0;; j++) {
            fscanf(file, "%c", &model[j]);
            if (model[j] == ',') {
                model[j] = '\0';
                break;
            }
        }
        fscanf(file, "%d, %d, %d, %d)", &cpuNum, &memorySize, &hardwareCost, &energyCost);
        fgetc(file);

        ServerInfo info(model, cpuNum, memorySize, hardwareCost, energyCost);
        receiver.push_back(info);
    }

    return 0;
}

int StdReader::ReadVMachineInfo(std::vector<VMInfo> &receiver) {

    int num;
    fscanf(file, "%d", &num);
    VMInfo info;

    char model[21];
    int cpuNum, memorySize, doubleNode;

    fgetc(file);
    for (int i = 0; i < num; i++) {
        fgetc(file);
        for (int j = 0;; j++) {
            fscanf(file, "%c", &model[j]);
            if (model[j] == ',') {
                model[j] = '\0';
                break;
            }
        }
        fscanf(file, "%d, %d, %d)", &cpuNum, &memorySize, &doubleNode);
        fgetc(file);

        VMInfo info(model, cpuNum, memorySize, doubleNode);
        receiver.push_back(info);

    }

    return 0;
}

int StdReader::ReadAllRequests(RequestsBatch &receiver) {
    int num;
    fscanf(file, "%d", &num);
    for (int i = 0; i < num; i++) {
        OneDayRequest req;
        ReadOneDayRequests(req);
        receiver.push_back(req);
    }
    return 0;
}

int StdReader::ReadOneDayRequests(OneDayRequest &receiver) {

    int num;
    fscanf(file, "%d", &num);

    fgetc(file);
    for (int i = 0; i < num; i++) {
        fgetc(file);
        Request req;
        char opStr[4];
        for (int j = 0;; j++) {
            fscanf(file, "%c", &opStr[j]);
            if (opStr[j] == ',') {
                opStr[j] = '\0';
                break;
            }
        }
        if (strcmp(opStr, "add") == 0) {
            req.op = ADD;
            fgetc(file);
            char vMachineModel[21];
            for (int j = 0;; j++) {
                fscanf(file, "%c", &vMachineModel[j]);
                if (vMachineModel[j] == ',') {
                    vMachineModel[j] = '\0';
                    break;
                }
            }
            req.vMachineModel = std::string(vMachineModel);
            fscanf(file, "%d)", &req.vMachineID);
        } else {
            req.op = DEL;
            fscanf(file, "%d)", &req.vMachineID);
        }
        fgetc(file);
        receiver.push_back(req);
    }

    return 0;
}

int StdWriter::write(ResultList &resultList) {
    int days = resultList.size();
    for (int i = 0; i < days; i++) {
        writeOneDayResult(resultList[i]);
    }
    return 0;
}

int StdWriter::writeOneDayResult(OneDayResult &oneDayResult) {
    fprintf(file, "(purchase, %d)\n", oneDayResult.purchaseVec.size());
    for (auto it:oneDayResult.purchaseVec) {
        fprintf(file, "(%s, %d)\n", it.serverName.c_str(), it.num);
    }
    fprintf(file, "(migration, %d)\n", oneDayResult.migrationList.size());
    for (int j = 0; j < oneDayResult.migrationList.size(); j++) {
        if (oneDayResult.migrationList[j].node != NODEAB) {
            fprintf(file, "(%d, %d, %c)\n", oneDayResult.migrationList[j].virtualID,
                    oneDayResult.migrationList[j].serverID, char(oneDayResult.migrationList[j].node + 'A'));
        } else {
            fprintf(file, "(%d, %d)\n", oneDayResult.migrationList[j].virtualID,
                    oneDayResult.migrationList[j].serverID);
        }
    }
    for (int j = 0; j < oneDayResult.deployList.size(); j++) {
        if (oneDayResult.deployList[j].node != NODEAB) {
            fprintf(file, "(%d, %c)\n",oneDayResult.deployList[j].serverID, char(oneDayResult.deployList[j].node + 'A'));
        } else {
            fprintf(file, "(%d)\n", oneDayResult.deployList[j].serverID);
        }
    }
    return 0;

}
