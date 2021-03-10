//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_READWRITER_H
#define HUAWEI_CODECRAFT_READWRITER_H

#include <vector>
#include <cstdio>

#include "server.h"

typedef struct{
    char op[4];
    char virtualMachineID[21];
}Request;

typedef std::vector<Request> OneDayRequest;

typedef struct{
    int requestNum;
    int dayNum;
    std::vector<OneDayRequest> bunch;
}RequestsBunch;

class RequestReader{
    virtual int ReadServersInfo(std::vector<ServerInfo> &receiver) =0;

    virtual int ReadBunchOfRequests(RequestsBunch &receiver) =0;
};

class ResultWriter{
    virtual int write()=0;
};

class FileReader: public RequestReader{
public:
    FILE* file=NULL;

    FileReader(char* filePath);

    FileReader()=delete;

    int ReadServersInfo(std::vector<ServerInfo> &receiver) override;

    int ReadBunchOfRequests(RequestsBunch &receiver) override;
};

#endif //HUAWEI_CODECRAFT_READWRITER_H
