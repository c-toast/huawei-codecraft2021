//
// Created by ctoast on 2021/3/10.
//

#include "readwriter.h"
#include "utils.h"
#include <cstdio>
#include <cstring>


FileReader::FileReader(char* filePath){
    file=fopen(filePath,"r");

}

int FileReader::ReadServersInfo(std::vector<ServerInfo> &receiver) {
    if(this->file==NULL){
        return -1;
    }
    int num;
    fscanf(this->file,"%d",&num);
    ServerInfo info;

    fgetc(this->file);
    for(int i=0;i<num;i++) {
        fgetc(this->file);
        for (int j = 0;; j++) {
            fscanf(this->file, "%c", &info.model[j]);
            if (info.model[j] == ',') {
                info.model[j] = '\0';
                break;
            }
        }
        fscanf(this->file, "%d, %d, %d, %d)", &info.cpuNum, &info.memorySize, &info.hardwareCost, &info.energyCost);
        fgetc(this->file);

        receiver.push_back(info);
    }

    return 0;
}

int FileReader::ReadVMachineInfo(std::vector<VirtualMachineInfo> &receiver) {
    if(this->file==NULL){
        return -1;
    }
    int num;
    fscanf(this->file,"%d",&num);
    VirtualMachineInfo info;

    fgetc(this->file);
    for(int i=0;i<num;i++) {
        fgetc(this->file);
        for (int j = 0;; j++) {
            fscanf(this->file, "%c", &info.model[j]);
            if (info.model[j] == ',') {
                info.model[j] = '\0';
                break;
            }
        }
        fscanf(this->file, "%d, %d, %d)", &info.cpuNum, &info.memorySize, &info.doubleNode);
        fgetc(this->file);
        receiver.push_back(info);

    }

    return 0;
}

int FileReader::ReadBunchOfRequests(RequestsBunch &receiver) {
    int num;
    fscanf(this->file,"%d",&num);
    for(int i=0;i<num;i++) {
        OneDayRequest req;
        ReadOneDayRequests(req);
    }
    return 0;
}

int FileReader::ReadOneDayRequests(OneDayRequest &receiver) {
    if(this->file==NULL){
        return -1;
    }
    int num;
    fscanf(this->file,"%d",&num);

    fgetc(this->file);
    for(int i=0;i<num;i++) {
        fgetc(this->file);
        Request req;
        char opStr[4];
        for (int j = 0;; j++) {
            fscanf(this->file, "%c", &opStr[j]);
            if (opStr[j] == ',') {
                opStr[j] = '\0';
                break;
            }
        }
        if(strcmp(opStr,"add")==0){
            req.op=ADD;
            fgetc(this->file);
            for (int j = 0;; j++) {
                fscanf(this->file, "%c", &req.vMachineModel[j]);
                if (req.vMachineModel[j] == ',') {
                    req.vMachineModel[j] = '\0';
                    break;
                }
            }
            fscanf(this->file, "%d)", &req.vMachineID);
        }
        else{
            req.op=DEL;
            fscanf(this->file, "%d)", &req.vMachineID);
        }
        fgetc(this->file);
        receiver.push_back(req);
    }

    return 0;
}








