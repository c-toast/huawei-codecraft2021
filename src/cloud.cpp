//
// Created by ctoast on 2021/3/11.
//

#include "cloud.h"
#include "utils.h"

//the type should be Cloud*
RatioCloud* globalCloud=new RatioCloud(3);

RatioCloud::RatioCloud(int number){
    ratioNum=number;
    serverID=0;
    for (int i=0;i<number;i++){
        std::vector<ServerInfo> serverInfoListInRatio;
        std::vector<ServerObj> serverObjListInRatio;
        std::vector<ServerInfo> serverCandidateListInRatio;
        //Resource initEmptyRsesource(0,0);
        serverInfoList.push_back(serverInfoListInRatio);
        serverObjList.push_back(serverObjListInRatio);
        serverCandidateList.push_back(serverCandidateListInRatio);
        //cloudReminingResource.push_back(initEmptyRsesource);
    }
}

int RatioCloud::computeCost(int vmID, VMInfo machineInfo, ServerInfo serverInfo){
    int hardwareCost, energyCost;
    int serverCpu, serverMemory, vmCpu, vmMemory, vmDayNumber;

    serverInfo.getCpuNum(serverCpu);
    serverInfo.getMemorySize(serverMemory);
    serverInfo.getHardwareCost(hardwareCost);
    serverInfo.getEnergyCost(energyCost);

    machineInfo.getCpuNum(vmCpu);
    machineInfo.getMemorySize(vmMemory);
    vmDayNumber=vmObjAddDelDate[vmID][1]-vmObjAddDelDate[vmID][0];

    int cost=(vmCpu/serverCpu + vmMemory/serverMemory)*(hardwareCost + vmDayNumber*energyCost);
    return cost;
}

int RatioCloud::calculateRatio(int ratio){
    /*/
    [4, infin):     0
    [3, 4):         1
    [2, 3):         2
    [1.5, 2):       3
    [1, 1.5):       4
    [0.66, 1):      5
    [0.5, 0.66):    6
    [0.33, 0.5):    7
    [0.25, 0.33):   8
    [0, 0.25):      9
    if(ratio>=4)    return 0;
    if(ratio>=3)    return 1;
    if(ratio>=2)    return 2;
    if(ratio>=1.5)  return 3;
    if(ratio>=1)    return 4;
    if(ratio>=0.66) return 5;
    if(ratio>=0.5)  return 6;
    if(ratio>=0.33) return 7;
    if(ratio>=0.25) return 8;
    return 9;
    /*/

    /*/
    [2, infin): 0
    [0.5, 2):   1
    [0, 0.5):   2
     */
    if(ratio>=2)   return 0;
    if(ratio>=0.5) return 1;
    return 2;
}

int RatioCloud::serverCalculateRatio(ServerInfo &serverInfo){
    int cpuNum, memory;
    serverInfo.getCpuNum(cpuNum);
    serverInfo.getMemorySize(memory);
    int ratio=cpuNum/memory;
    return calculateRatio(ratio);
}

int RatioCloud::VMInfoCalculateRatio(VMInfo &vmInfo){
    int cpuNum, memory;
    vmInfo.getCpuNum(cpuNum);
    vmInfo.getMemorySize(memory);
    int ratio=cpuNum/memory;
    return calculateRatio(ratio);
}

int RatioCloud::addServerObj(ServerInfo &serverInfo) {
    int ratio=serverCalculateRatio(serverInfo);
    ServerObj obj(serverInfo,serverID++,serverObjList[ratio].size());
    serverObjList[ratio].push_back(obj);
    return 0;
}

// already know which server to add vm
int RatioCloud::addVMObj(ServerObj &serverObj, int nodeIndex, std::string vmModel, int vmID, int ratio) {
    if(serverObj.ID < 0 || serverObj.ID > serverID){
        LOGE("serverObj id is wrong");
        exit(-1);
    }
    VMInfo& vmInfo=vmInfoMap[vmModel];
    VMObj vmObj(vmInfo,vmID);

    int doubleNode;vmInfo.getDoubleNode(doubleNode);
    Resource requiredRes;
    vmInfo.getRequiredResourceForOneNode(requiredRes);

    if(doubleNode==1){
        serverObj.deployVM(0, vmObj);
        serverObj.deployVM(1, vmObj);
        //cloudReminingResource[ratio].allocResource(requiredRes);
        //cloudReminingResource[ratio].allocResource(requiredRes);
    }else{
        serverObj.deployVM(nodeIndex,vmObj);
        //cloudReminingResource[ratio].allocResource(requiredRes);
    }

    vmObjMap.insert({vmID, vmObj});
    return 0;
}

int RatioCloud::delVMObj(int machineID) {
    auto machineIterator=vmObjMap.find(machineID);
    if(machineIterator == vmObjMap.end()){
        LOGE("can not find the vmObj in vmObj map");
        exit(-1);
    }
    auto vmObj=machineIterator->second;
    VMInfo vmInfo;
    vmObj.getInfo(vmInfo);
    //Resource requiredRes;
    //vmInfo.getRequiredResourceForOneNode(requiredRes);

    int ratio, ratioID;
    vmObj.getDeployRatioID(ratioID);
    ratio=VMInfoCalculateRatio(vmInfo);
    if(ratioID<0||ratioID>serverObjList[ratio].size()-1){
        LOGE("vmObj deployed id is wrong");
        exit(-1);
    }
    auto& server=serverObjList[ratio][ratioID];
    vmObjMap.erase(machineIterator);
    
    for(auto nodeIndex:vmObj.deployNodes){
        server.delVM(nodeIndex, vmInfo);
        //cloudReminingResource[ratio].freeResource(requiredRes);
    }

    return 0;
}