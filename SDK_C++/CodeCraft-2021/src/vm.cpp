//
// Created by ctoast on 2021/3/13.
//

#include "vm.h"

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

//int VMObj::deployInServer(int serverNode) {
//    deployNodes.push_back(serverNode);
//    return 0;
//}
//
//int VMObj::deployInCloud(int serverId) {
//    deployServerID=serverId;
//    return 0;
//}

int VMObj::getDeployNode() {
    if(deployNodes.empty()){
        LOGE("VMObj::getDeployNode: the obj have not been deploy in node");
        exit(-1);
    }
    if(info.doubleNode==1){
        return NODEAB;
    }else{
        return deployNodes[0];
    }
}
