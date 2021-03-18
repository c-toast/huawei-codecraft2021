//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_CLOUD_H
#define HUAWEI_CODECRAFT_CLOUD_H

#include <vector>
#include <map>
#include "server.h"
#include "vm.h"
#include <unordered_map>
#include <queue>

class Cloud{
public:
    std::map<std::string,VMInfo> vmInfoMap; //[model]info

    std::map<int,VMObj> vmObjMap;//[id]obj.

    virtual int addServerObj(ServerInfo &serverInfo) =0;

    //addVMObj is supposed to automatically handle with the double node situation and single node situation
    //in the former case, nodeIndex is useless
    virtual int addVMObj(ServerObj &serverObj, int nodeIndex, std::string vmModel, int vmID, int ratio) =0;

    virtual int delVMObj(int machineID)=0;

};

class RatioCloud: public Cloud{
public:
    RatioCloud()=default;

    // how many ratios are there
    // [0,1,...,ratioNum-1]
    int ratioNum;

    int serverID;

    std::map<std::string,ServerInfo> serverInfoMap; //[model]info

    // serverInfoList is the vector of different serverInfoListInRatio
    std::vector< std::vector<ServerInfo> > serverInfoList;

    // serverObjListRatio is the vector of different serverObjListInRatio
    std::vector< std::vector<ServerObj> > serverObjList;

    std::vector< std::vector<ServerInfo> > serverCandidateList;

    // cloudReminingResource in every serverObjListInRatio, but we may not use it
    //std::vector<Resource> cloudReminingResource;

    // [ID]: <add, del>
    std::unordered_map<int, std::vector<int> > vmObjAddDelDate;

    // [serverModel]: <vmID1, vmID2, ...>
    std::map<std::string, std::vector<int> > migrateList;

    RatioCloud(int number);

    int calculateRatio(int ratio);

    int serverCalculateRatio(ServerInfo &serverInfo);
    
    int VMInfoCalculateRatio(VMInfo &vmInfo);

    int computeCost(int vmID, VMInfo machineInfo, ServerInfo serverInfo);

    int addServerObj(ServerInfo &serverInfo) override;

    int addVMObj(ServerObj &serverObj, int nodeIndex, std::string vmModel, int vmID, int ratio) override;

    int delVMObj(int machineID) override;

};

#endif //HUAWEI_CODECRAFT_CLOUD_H
