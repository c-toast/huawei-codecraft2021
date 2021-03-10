//
// Created by ctoast on 2021/3/10.
//

#include "readwriter.h"
#include <cstdio>


class FileReader: public RequestReader{
    FILE* file=NULL;

    FileReader(char* filePath);

    FileReader()=delete;

    int ReadServersInfo(std::vector<ServerInfo> &receiver) override;

    int ReadBunchOfRequests(RequestsBunch &receiver) override;
};

FileReader::FileReader(char* filePath){
    file=fopen(filePath,"r");
}

int FileReader::ReadServersInfo(std::vector<ServerInfo> &receiver) {
    if(this->file==NULL){
        return -1;
    }
    

    return 0;
}

int FileReader::ReadBunchOfRequests(RequestsBunch &receiver) {
    return 0;
}






