//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_READWRITER_H
#define HUAWEI_CODECRAFT_READWRITER_H

#include <vector>
#include <cstdio>

#include "server.h"
#include "virtual-machine.h"

#define ADD 1
#define DEL 0

typedef struct{
    int op;
    char vMachineModel[21];
    int vMachineID;
}Request;

typedef std::vector<Request> OneDayRequest;

typedef struct{
    int requestNum;
    int dayNum;
    std::vector<OneDayRequest> bunch;
}RequestsBunch;

typedef struct{
    char serverName;
    int num;
}Purchase;

typedef struct{
    int tmp;
}Migration;

typedef struct{
    int serverID;
    char node;
}Deploy;

typedef struct{
    std::vector<Purchase> purchaseList;
    std::vector<Migration> migrationList;
    std::vector<Deploy> deployList;
}Result;

class RequestReader{
    virtual int ReadServersInfo(std::vector<ServerInfo> &receiver) =0;

    virtual int ReadVMachineInfo(std::vector<VirtualMachineInfo> &receiver) =0;

    virtual int ReadBunchOfRequests(RequestsBunch &receiver) =0;
};

class ResultWriter{
    virtual int write(Result& res)=0;
};

class FileReader: public RequestReader{
public:
    FILE* file=NULL;

    FileReader(char* filePath);

    FileReader()=delete;

    int ReadServersInfo(std::vector<ServerInfo> &receiver) override;

    int ReadVMachineInfo(std::vector<VirtualMachineInfo> &receiver) override ;

    int ReadBunchOfRequests(RequestsBunch &receiver) override;

    int ReadOneDayRequests(OneDayRequest &receiver);
};

#endif //HUAWEI_CODECRAFT_READWRITER_H
