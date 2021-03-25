//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_NEWSERVERBUYER_H
#define HUAWEI_CODECRAFT_NEWSERVERBUYER_H

#include "map"
#include "server.h"
#include "vm.h"
#include "readwriter.h"

class NewServerBuyer{
public:
    std::map<double,std::vector<ServerInfo*>> Clusters; //[radio][servercluster]

    int buyAndDeploy(std::vector<VMObj*> &unhandledVMObj);

    int learnModelInfo();

    int learnPosteriorInfo(RequestsBatch &requestsBunch);

    int buyAndDeployDoubleNode(std::vector<VMObj*> &unhandledVMObj);

    int buyAndDeploySingleNode(std::vector<VMObj*> &singleNodeVMObj);

    int classify(std::vector<VMObj *> &vmObjVec,std::map<double,std::vector<VMObj*>>& receiver);

    int movVMObjToNewServerObj(ServerObj *oldObj, ServerObj *newObj);
};


#endif //HUAWEI_CODECRAFT_NEWSERVERBUYER_H
