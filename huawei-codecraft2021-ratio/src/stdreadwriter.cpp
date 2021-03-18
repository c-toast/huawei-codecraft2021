//
// Created by ctoast on 2021/3/12.
//

#include "readwriter.h"
#include "cloud.h"
#include "cstring"
#include <iostream>

extern RatioCloud* globalCloud;

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

int StdReader::ReadBunchOfRequests(RequestsBunch &receiver) {
    int num;
    scanf("%d",&num);
    receiver.dayNum = num;
    for(int i=0;i<num;i++) {
        OneDayRequest req;
        // add today and totalDay to build a unordered_map of VMAddDay and VMDelDay
        ReadOneDayRequests(req, i, num);
        receiver.bunch.push_back(req);
    }
    return 0;
}

int StdReader::ReadOneDayRequests(OneDayRequest &receiver, int today, int totalDay) {
    std::unordered_map<int, std::vector<int> > &vmObjDate=globalCloud->vmObjAddDelDate;

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
            // add into unordered_map
            vmObjDate[req.vMachineID]={today, totalDay};
        }
        else{
            req.op=DEL;
            scanf("%d)", &req.vMachineID);
            // add into unordered_map
            vmObjDate[req.vMachineID][1]=today;
        }
        getchar();
        receiver.push_back(req);
    }

    return 0;
}

int StdWriter::write(ResultList& resultList) {
    int days = resultList.size();
    for (int i = 0; i < days; i++) {
        std::cout << "(purchase, " << resultList[i].purchaseMap.size() << ")" << std::endl;
        for (auto it:resultList[i].purchaseMap) {
            std::cout << "(" << it.first << ", " << it.second << ")" << std::endl;
        }
        std::cout << "(migration, " << resultList[i].migrationList.size() << ")" << std::endl;
        for (int j = 0; j < resultList[i].migrationList.size(); j++) {
            if (resultList[i].migrationList[j].node!=2)
                std::cout << "("<<resultList[i].migrationList[j].virtualID << ", " << resultList[i].migrationList[j].serverID << ", " << char(resultList[i].migrationList[j].node + 'A') << ")" << std::endl;
            else
                std::cout << "(" << resultList[i].migrationList[j].virtualID << ", " << resultList[i].migrationList[j].serverID  << ")" << std::endl;
        }
        for (int j = 0; j < resultList[i].deployList.size(); j++) {
            if (resultList[i].deployList[j].node!=-1)
                std::cout << "(" << resultList[i].deployList[j].serverID << ", " << char(resultList[i].deployList[j].node+'A') << ")" << std::endl;
            else
                std::cout << "(" << resultList[i].deployList[j].serverID << ")" << std::endl;
        }
    }
    return 0;
}
