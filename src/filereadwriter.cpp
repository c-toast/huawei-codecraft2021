//
// Created by ctoast on 2021/3/10.
//

#include "readwriter.h"
#include "utils.h"
#include <cstdio>
#include <fstream>



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

int FileReader::ReadBunchOfRequests(RequestsBunch &receiver) {
    return 0;
}






