//
// Created by ctoast on 2021/3/12.
//

#include "readwriter.h"
#include "cstring"

int StdReader::ReadServersInfo(std::vector<ServerInfo> &receiver) {

    int num;
    scanf("%d",&num);
    ServerInfo info;

    getchar();
    for(int i=0;i<num;i++) {
        getchar();
        for (int j = 0;; j++) {
            scanf("%c", &info.model[j]);
            if (info.model[j] == ',') {
                info.model[j] = '\0';
                break;
            }
        }
        scanf("%d, %d, %d, %d)", &info.cpuNum, &info.memorySize, &info.hardwareCost, &info.energyCost);
        getchar();

        receiver.push_back(info);
    }

    return 0;
}

int StdReader::ReadVMachineInfo(std::vector<VirtualMachineInfo> &receiver) {

    int num;
    scanf("%d",&num);
    VirtualMachineInfo info;

    getchar();
    for(int i=0;i<num;i++) {
        getchar();
        for (int j = 0;; j++) {
            scanf("%c", &info.model[j]);
            if (info.model[j] == ',') {
                info.model[j] = '\0';
                break;
            }
        }
        scanf("%d, %d, %d)", &info.cpuNum, &info.memorySize, &info.doubleNode);
        getchar();
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
        ReadOneDayRequests(req);
        receiver.bunch.push_back(req);
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
            for (int j = 0;; j++) {
                scanf("%c", &req.vMachineModel[j]);
                if (req.vMachineModel[j] == ',') {
                    req.vMachineModel[j] = '\0';
                    break;
                }
            }
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

