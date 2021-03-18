//
// Created by ctoast on 2021/3/13.
//

#include "vm.h"

int VMInfo::getModel(std::string& receiver){receiver=model;return 0;}

int VMInfo::getCpuNum(int& receiver){receiver=cpuNum;return 0;}

int VMInfo::getMemorySize(int& receiver){receiver=memorySize;return 0;}

int VMInfo::getDoubleNode(int &receiver){ receiver=doubleNode;return 0;}

int VMInfo::getRequiredResourceForOneNode(Resource &receiver) const {
    if(doubleNode){
        receiver.memorySize=memorySize/2;
        receiver.cpuNum=cpuNum/2;
    }else{
        receiver.memorySize=memorySize;
        receiver.cpuNum=cpuNum;
    }
    return 0;
}

int VMObj::getInfo(VMInfo &receiver) {
    receiver=info;
    return 0;
}

int VMObj::getID(int &receiver) {
    receiver=id;
    return 0;
}

int VMObj::getDeployServerID(int &receiver) {
    receiver=deployServerID;
    return 0;
}

int VMObj::getDeployRatioID(int &receiver){
    receiver=deployRatioID;
    return 0;
}

// 0:A, 1:B, 2:A+B
int VMObj::getDeployNode(int &receiver){
    if(deployNodes.size()==2)
        receiver=2;
    else
        receiver=deployNodes[0];
    return 0;
}


int VMObj::deploy(int serverId, int serverNode, int ratioID) {
    if(deployServerID!=-1&&deployServerID!=serverId){
        LOGE("the VMObj was deployed on different server");
        exit(-1);
    }
    deployServerID=serverId;
    deployNodes.push_back(serverNode);
    deployRatioID=ratioID;
    return 0;
}

