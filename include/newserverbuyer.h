//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_NEWSERVERBUYER_H
#define HUAWEI_CODECRAFT_NEWSERVERBUYER_H

#include "map"
#include "server.h"
#include "vm.h"
#include "readwriter.h"

class NewServerBuyer {
public:
    std::map<double, std::vector<ServerInfo *>> Clusters; //[radio][servercluster]

    std::vector<ServerInfo *> allServerInfos;

    std::map<VMObj*,std::vector<double>> voteResMap;

    std::vector<double> batchVoteRes;

    std::map<int,std::vector<double>> batchVoteResMap;//[id]res

    int buyAndDeploy(std::vector<VMObj *> &unhandledVMObj);

    ServerObj *createASuitableServer(std::vector<ServerInfo *> &candidateServers,
                                     std::vector<VMObj *> &unhandledVMObj);

    int DeployVMInServer(ServerObj *newServerObj, std::vector<VMObj *> &unhandledVMObj);

    int normalize(std::vector<double> &vec);

    int voteForServer(VMObj *vmObj, std::vector<ServerInfo *> &candidateServers, std::vector<double> &voteRes);

    int voteForServer(VMInfo vmInfo, std::vector<ServerInfo *> &candidateServers, std::vector<double> &voteRes);



    int learnModelInfo();


    int classify(std::vector<VMObj *> &vmObjVec, std::map<double, std::vector<VMObj *>> &receiver);

    int initWhenNewBatchCome(RequestsBatch &newBatch, RequestsBatch &oldBatch);

    int initWhenNewDayStart(OneDayRequest &currentDayReq);

    int init();
};


#endif //HUAWEI_CODECRAFT_NEWSERVERBUYER_H
