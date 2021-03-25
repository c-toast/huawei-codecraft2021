//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_CLOUD_H
#define HUAWEI_CODECRAFT_CLOUD_H

#include <vector>
#include <map>

#include "server.h"
#include "vm.h"
#include "server.h"
#include "vm.h"

class Cloud{
public:
    std::map<std::string,VMInfo> vmInfoMap; //[model]info

    std::vector<ServerObj> serverObjList; //obj is in their id order in this list

    std::map<int,VMObj> vmObjMap;//[id]obj.

    virtual int addServerObj(ServerInfo &serverInfo) =0;

    //addVMObj is supposed to automatically handle with the double node situation and single node situation
    //in the former case, nodeIndex is useless
    virtual int addVMObj(int serverObjID, int nodeIndex, std::string vmModel, int vmID) =0;

    virtual int delVMObj(int machineID)=0;

    //the following three method only get the copy, but not the real obj storing in the cloud
    int getServerObjById(int id, ServerObj& receiver);

    int getVMObjById(int id, VMObj& receiver);

    int getVMInfoByModel(std::string model, VMInfo& receiver);

    //virtual int MovMachine(int from,int to,VirtualMachine& machine)=0;

    //virtual vector<int> FindServersByCondition()=0;
};

class SimpleCloud: public Cloud{
public:
    SimpleCloud()=default;

    std::vector<ServerInfo> serverInfoList;

	std::vector<ServerInfo> serverSortList;

    int addServerObj(ServerInfo &serverInfo) override;

    int addVMObj(int serverObjID, int nodeIndex, std::string vmModel, int vmID) override;

    int delVMObj(int machineID) override;
};

#endif //HUAWEI_CODECRAFT_CLOUD_H
