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

    std::map<std::string,ServerInfo> serverInfoMap; //[model]info

    std::vector<ServerObj*> serverObjList; //[id]obj

    std::map<int,VMObj*> vmObjMap;//[id]obj.

    Cloud()=default;

    int createServerObj(ServerInfo &serverInfo);

    int deployServerObj(ServerObj C);

    VMObj * createVMObj(int vmID, std::string model);

    //nodeIndex can be NODEA NODEB or NODEAB
    int deployVMObj(int serverObjID, int nodeIndex, int vmID);

    int deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj);

    int delVMObjFromCloud(int vmID);

    int delVMObjFromServerObj(int vmID);

    int renewServerID(int start);

    int total_day;

    Resource ServerResource=Resource(0,0);
    Resource VMResource=Resource(0,0);
};



#endif //HUAWEI_CODECRAFT_CLOUD_H
