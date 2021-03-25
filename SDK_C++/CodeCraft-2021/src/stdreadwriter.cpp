//
// Created by ctoast on 2021/3/12.
//

#include "readwriter.h"
#include "cstring"
#include <iostream>

int StdReader::ReadServersInfo(std::vector<ServerInfo> &receiver) {

    int num;
    scanf("%d",&num);
    char model[21];
    int cpuNum,memorySize,hardwareCost,energyCost;

    getchar();
    for(int i=0;i<num;i++) {
        getchar();
        for (int j = 0;; j++) {
            scanf("%c", &model[j]);
            if (model[j] == ',') {
                model[j] = '\0';
                break;
            }
        }
        scanf("%d, %d, %d, %d)", &cpuNum, &memorySize, &hardwareCost, &energyCost);
        getchar();

        ServerInfo info(model,cpuNum,memorySize,hardwareCost,energyCost);
        receiver.push_back(info);
    }

    return 0;
}

int StdReader::ReadVMachineInfo(std::vector<VMInfo> &receiver) {

    int num;
    scanf("%d",&num);
    VMInfo info;

    char model[21];
    int cpuNum,memorySize,doubleNode;

    getchar();
    for(int i=0;i<num;i++) {
        getchar();
        for (int j = 0;; j++) {
            scanf("%c", &model[j]);
            if (model[j] == ',') {
                model[j] = '\0';
                break;
            }
        }
        scanf("%d, %d, %d)", &cpuNum, &memorySize, &doubleNode);
        getchar();

        VMInfo info(model,cpuNum,memorySize,doubleNode);
        receiver.push_back(info);

    }

    return 0;
}

int StdReader::ReadAllRequests(RequestsBatch &receiver) {
    int num;
    scanf("%d",&num);
    for(int i=0;i<num;i++) {
        OneDayRequest req;
        ReadOneDayRequests(req);
        receiver.push_back(req);
    }
    return 0;
}

int StdReader::ReadOneDayRequests(OneDayRequest &receiver) {

    int num;
    scanf("%d",&num);

    getchar();
    for(int i=0;i<num;i++) {
        getchar();
        Request req;
        char opStr[4];
        for (int j = 0;; j++) {
            scanf("%c", &opStr[j]);
            if (opStr[j] == ',') {
                opStr[j] = '\0';
                break;
            }
        }
        if(strcmp(opStr,"add")==0){
            req.op=ADD;
            getchar();
            char vMachineModel[21];
            for (int j = 0;; j++) {
                scanf("%c", &vMachineModel[j]);
                if (vMachineModel[j] == ',') {
                    vMachineModel[j] = '\0';
                    break;
                }
            }
            req.vMachineModel=std::string(vMachineModel);
            scanf("%d)", &req.vMachineID);
        }
        else{
            req.op=DEL;
            scanf("%d)", &req.vMachineID);
        }
        getchar();
        receiver.push_back(req);
    }

    return 0;
}

int StdWriter::write(ResultList& resultList) {
    int days = resultList.size();
    for (int i = 0; i < days; i++) {
        std::cout << "(purchase, " << resultList[i].purchaseVec.size() << ")" << std::endl;
        for (auto it:resultList[i].purchaseVec) {
            std::cout << "(" << it.serverName << ", " << it.num << ")" << std::endl;
        }
        std::cout << "(migration, " << resultList[i].migrationList.size() << ")" << std::endl;
        for (int j = 0; j < resultList[i].migrationList.size(); j++) {
            if (resultList[i].migrationList[j].node!=NODEAB)
                std::cout << "("<<resultList[i].migrationList[j].virtualID << ", " << resultList[i].migrationList[j].serverID << ", " << char(resultList[i].migrationList[j].node+'A') << ")" << std::endl;
            else
                std::cout << "(" << resultList[i].migrationList[j].virtualID << ", " << resultList[i].migrationList[j].serverID  << ")" << std::endl;
        }
        for (int j = 0; j < resultList[i].deployList.size(); j++) {
            if (resultList[i].deployList[j].node!=NODEAB)
                std::cout << "(" << resultList[i].deployList[j].serverID << ", " << char(resultList[i].deployList[j].node+'A') << ")" << std::endl;
            else
                std::cout << "(" << resultList[i].deployList[j].serverID << ")" << std::endl;
        }
    }
    return 0;
}

int StdWriter::writeOneDayResult(OneDayResult &oneDayResult) {
    std::cout << "(purchase, " << oneDayResult.purchaseVec.size() << ")" << std::endl;
    for (auto it:oneDayResult.purchaseVec) {
        std::cout << "(" << it.serverName << ", " << it.num << ")" << std::endl;
    }
    std::cout << "(migration, " << oneDayResult.migrationList.size() << ")" << std::endl;
    for (int j = 0; j < oneDayResult.migrationList.size(); j++) {
        if (oneDayResult.migrationList[j].node!=NODEAB)
            std::cout << "("<<oneDayResult.migrationList[j].virtualID << ", " << oneDayResult.migrationList[j].serverID << ", " << char(oneDayResult.migrationList[j].node+'A') << ")" << std::endl;
        else
            std::cout << "(" << oneDayResult.migrationList[j].virtualID << ", " << oneDayResult.migrationList[j].serverID  << ")" << std::endl;
    }
    for (int j = 0; j < oneDayResult.deployList.size(); j++) {
        if (oneDayResult.deployList[j].node!=NODEAB)
            std::cout << "(" << oneDayResult.deployList[j].serverID << ", " << char(oneDayResult.deployList[j].node+'A') << ")" << std::endl;
        else
            std::cout << "(" << oneDayResult.deployList[j].serverID << ")" << std::endl;
    }
    return 0;

}
