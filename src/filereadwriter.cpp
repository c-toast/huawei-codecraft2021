//
// Created by ctoast on 2021/3/10.
//

#include "readwriter.h"
#include "utils.h"
#include <cstdio>
#include <cstring>
#include <iostream>


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
	receiver.dayNum = num;
    for(int i=0;i<num;i++) {
        OneDayRequest req;
        ReadOneDayRequests(req);
		receiver.bunch.push_back(req);
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

int StdWriter::write(ResultList& resultList) {
	int days = resultList.size();
	for (int i = 0; i < days; i++) {
		std::cout <<"(purchase, "<< resultList[i].purchaseList.size() <<")"<< std::endl;
		for (int j = 0; j < resultList[i].purchaseList.size(); j++) {
			std::cout << "(" << std::string(resultList[i].purchaseList[j].serverName) << ", " << resultList[i].purchaseList[j].num << ")"<<std::endl;
		}
		std::cout << "(migration, " << resultList[i].migrationList.size() << ")" << std::endl;
		for (int j = 0; j < resultList[i].migrationList.size(); j++) {
			if (resultList[i].migrationList[j].node)
				std::cout << "("<<resultList[i].migrationList[j].virtualID << ", " << resultList[i].migrationList[j].serverID << ", " << resultList[i].migrationList[j].node << ")" << std::endl;
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


