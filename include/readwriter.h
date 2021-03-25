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

typedef std::vector<OneDayRequest> RequestsBatch;

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

class StdWriter{
public:
	int write(ResultList& resultList);

	int writeOneDayResult(OneDayResult& oneDayResult);
};


class StdReader{
public:
    StdReader()=default;

    int ReadServersInfo(std::vector<ServerInfo> &receiver);

    int ReadVMachineInfo(std::vector<VMInfo> &receiver);

    int ReadAllRequests(RequestsBatch &receiver);

    int ReadOneDayRequests(OneDayRequest &receiver);
};

#endif //HUAWEI_CODECRAFT_READWRITER_H
