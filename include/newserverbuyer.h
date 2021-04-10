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

    std::vector<double> batchVoteRes;

    int buyAndDeploy(std::vector<VMObj *> &unhandledVMObj);

    ServerObj *createASuitableServer(std::vector<ServerInfo *> &candidateServers,
                                     std::vector<VMObj *> &unhandledVMObj);

    int DeployVMInServer(ServerObj *newServerObj, std::vector<VMObj *> &unhandledVMObj);

    int normalize(std::vector<double> &vec);

    int voteForServer(VMObj *vmObj, std::vector<ServerInfo *> &candidateServers, std::vector<double> &voteRes);

    int voteForServer(VMInfo vmInfo, std::vector<ServerInfo *> &candidateServers, std::vector<double> &voteRes);


    std::map<VMObj*,std::vector<double>> voteResMap;



    int learnModelInfo();

    int learnPosteriorInfo(RequestsBatch &requestsBunch);

    int buyAndDeployDoubleNode(std::vector<VMObj *> &unhandledVMObj);

    int buyAndDeploySingleNode(std::vector<VMObj *> &singleNodeVMObj);

    int classify(std::vector<VMObj *> &vmObjVec, std::map<double, std::vector<VMObj *>> &receiver);

    int movVMObjToNewServerObj(ServerObj *oldObj, ServerObj *newObj);

    int initWhenNewBatchCome(RequestsBatch &batch);

    int initWhenNewDayStart();

    int init();
};


#endif //HUAWEI_CODECRAFT_NEWSERVERBUYER_H
