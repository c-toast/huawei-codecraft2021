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
    //deployVMObj is supposed to automatically handle with the double node situation and single node situation
    //in the former case, nodeIndex is useless
    virtual int deployVMObj(int serverObjID, int nodeIndex, int vmID) =0;

    virtual int delVMObj(int vmID)=0;

    //the following three method only get the copy, but not the real obj storing in the cloud
    int getServerObjById(int id, ServerObj& receiver);

    int getVMObjById(int id, VMObj& receiver);

    int getVMInfoByModel(std::string model, VMInfo& receiver);

    //virtual int MovMachine(int from,int to,VirtualMachine& machine)=0;

    //virtual vector<int> FindServersByCondition()=0;
};

class SimpleCloud: public Cloud{
public:
    std::map<std::string,VMInfo> vmInfoMap; //[model]info

    std::map<std::string,ServerInfo> serverInfoMap; //[model]info

    std::vector<ServerObj*> serverObjList; //[id]obj

    std::map<int,VMObj*> vmObjMap;//[id]obj.

    SimpleCloud()=default;

    int createServerObj(ServerInfo &serverInfo);

    int deployServerObj(ServerObj C);

    VMObj * createVMObj(int vmID, std::string model);

    //nodeIndex can be NODEA NODEB or NODEAB
    int deployVMObj(int serverObjID, int nodeIndex, int vmID);

    int deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj);

    int delVMObj(int vmID) override;

    int MigrateVMObj(int vmID);

    int renewServerID(int start);
};

#endif //HUAWEI_CODECRAFT_CLOUD_H
