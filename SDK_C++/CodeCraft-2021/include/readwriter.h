//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_READWRITER_H
#define HUAWEI_CODECRAFT_READWRITER_H

#include <vector>
#include <cstdio>
#include <map>

#include "server.h"
#include "vm.h"

#define ADD 1
#define DEL 0

typedef struct{
    int op;
    std::string vMachineModel;
    int vMachineID;
}Request;

typedef std::vector<Request> OneDayRequest;

typedef struct{
    int requestNum;
    int dayNum;
    std::vector<OneDayRequest> bunch;
}RequestsBunch;

typedef struct{
    std::string serverName;
    int num;
}Purchase;

typedef struct{
	int serverID;
	int virtualID;
	int node;
}Migration;

typedef struct{
    int serverID;
    int node;
}Deploy;

typedef struct{
    std::vector<Purchase> purchaseVec;
    std::vector<Migration> migrationList;
    std::vector<Deploy> deployList;
}OneDayResult;

typedef std::vector<OneDayResult> ResultList;

class RequestReader{
public:
    virtual int ReadServersInfo(std::vector<ServerInfo> &receiver) =0;

    virtual int ReadVMachineInfo(std::vector<VMInfo> &receiver) =0;

    virtual int ReadBunchOfRequests(RequestsBunch &receiver) =0;
};

class ResultWriter{
public:
    virtual int write(ResultList& resultList)=0;
};

class StdWriter : public ResultWriter {
public:
	int write(ResultList& resultList) override;
};


class StdReader: public RequestReader{
public:
    StdReader()=default;

    int ReadServersInfo(std::vector<ServerInfo> &receiver) override;

    int ReadVMachineInfo(std::vector<VMInfo> &receiver) override ;

    int ReadBunchOfRequests(RequestsBunch &receiver) override;

    int ReadOneDayRequests(OneDayRequest &receiver);
};

#endif //HUAWEI_CODECRAFT_READWRITER_H
