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

class CloudListener{
public:
    virtual int deployServerObj(ServerObj C){return 0;}

    virtual VMObj* createVMObj(int vmID, std::string model){return NULL;}

    virtual int deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj){return 0;}

    virtual int delVMObjFromCloud(int vmID){return 0;}

    virtual int moveVMObjFromServerObj(int vmID){return 0;}

    virtual int createServerObj(ServerInfo &serverInfo){return 0;}
};


class Cloud{
public:
    std::map<std::string,VMInfo> vmInfoMap; //[model]info

    std::map<std::string,ServerInfo> serverInfoMap; //[model]info

    std::vector<ServerObj*> serverObjList; //[id]obj

    std::map<int,VMObj*> vmObjMap;//[id]obj.

    Cloud()=default;

    std::map<std::string,std::vector<ServerObj*>> modelServerMap;

    std::vector<CloudListener*> beforelistenerList;

    int createServerObj(ServerInfo &serverInfo);

    int deployServerObj(ServerObj C);

    VMObj* createVMObj(int vmID, std::string model);

    int deployVMObj(int serverObjID, int nodeIndex, VMObj* vmObj);

    int delVMObjFromCloud(int vmID);

//    int eraseVMObj(int vmID);

    int moveVMObjFromServerObj(int vmID);

    int registerBeforeListener(CloudListener* h){beforelistenerList.push_back(h);return 0;}
};




#endif //HUAWEI_CODECRAFT_CLOUD_H
