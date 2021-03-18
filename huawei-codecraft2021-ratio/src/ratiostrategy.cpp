//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "cloud.h"
#include "utils.h"
#include "cstring"

extern RatioCloud* globalCloud;

int RatioStrategy::dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) {
    int dayNum=requestsBunch.dayNum;
    auto& bunch=requestsBunch.bunch;

    for(int i=0;i<dayNum;i++){
        purchaseMap.clear();
        OneDayResult oneDayRes;
        OneDayRequest& oneDayReq=bunch[i];

        MigrationFunction(oneDayRes,i);
        for(auto it:oneDayReq){
            if(it.op==ADD){
                HandleAdd(it,oneDayRes);
            }else{
                HandleDel(it,oneDayRes);
            }
        }
        receiver.push_back(oneDayRes);
    }
    return 0;
}

int RatioStrategy::MigrationFunction(OneDayResult &receiver, int today){
    int migrateNum=(globalCloud->vmObjMap.size()) / 1000 * 5;
    int mostNum=-1;
    std::string serverModel;
    // find the server we need to buy
    for(auto it=globalCloud->migrateList.begin(); it!=globalCloud->migrateList.end(); it++){
        if(it->second.size()>mostNum){
            mostNum=it->second.size();
            serverModel=it->first;
        }
    }
    auto it=globalCloud->migrateList.find(serverModel);
    std::vector<int>& migrateList=it->second;

    ServerInfo serverInfo=globalCloud->serverInfoMap[serverModel];
    globalCloud->addServerObj(serverInfo);
    int ratio=globalCloud->serverCalculateRatio(serverInfo);
    receiver.purchaseMap[serverModel]+=1;
    ServerObj& serverObj=globalCloud->serverObjList[ratio].back();
    // migrate
    for(int i=0; i<migrateNum && i<mostNum; i++){
        int machineID=migrateList[i];
        auto machineIterator=globalCloud->vmObjMap.find(machineID);
        auto vmObj=machineIterator->second;
        VMInfo vmInfo;
        vmObj.getInfo(vmInfo);
        if(serverObj.canDeployOnSingleNode(0,vmInfo)){
            int ratioID, oldNode;
            vmObj.getDeployRatioID(ratioID);
            vmObj.getDeployNode(oldNode);
            ServerObj& oldServer=globalCloud->serverObjList[ratio][ratioID];
            // release old server
            oldServer.delVM(oldNode, vmInfo);
            // new server deploy
            serverObj.deployVM(0,vmObj);
            // record migrate
            Migration migration;
            migration.node=0;
            migration.serverID=serverObj.ID;
            migration.virtualID=machineID;
            receiver.migrationList.push_back(migration);
            // update vmObjAddDelDate
            globalCloud->vmObjAddDelDate[machineID][0]=today;
        }
        if(serverObj.canDeployOnSingleNode(1,vmInfo)){
            int ratioID, oldNode;
            vmObj.getDeployRatioID(ratioID);
            vmObj.getDeployNode(oldNode);
            ServerObj& oldServer=globalCloud->serverObjList[ratio][ratioID];
            // release old server
            oldServer.delVM(oldNode, vmInfo);
            // new server deploy
            serverObj.deployVM(1,vmObj);
            // record migrate
            Migration migration;
            migration.node=1;
            migration.serverID=serverObj.ID;
            migration.virtualID=machineID;
            receiver.migrationList.push_back(migration);
            // update vmObjAddDelDate
            globalCloud->vmObjAddDelDate[machineID][0]=today;
        }
        if(serverObj.canDeployOnDoubleNode(vmInfo)){
            int ratioID;
            vmObj.getDeployRatioID(ratioID);
            ServerObj& oldServer=globalCloud->serverObjList[ratio][ratioID];
            // release old server
            oldServer.delVM(0, vmInfo);
            oldServer.delVM(1, vmInfo);
            // new server deploy
            serverObj.deployVM(0,vmObj);
            serverObj.deployVM(1,vmObj);
            // record migrate
            Migration migration;
            migration.node=2;
            migration.serverID=serverObj.ID;
            migration.virtualID=machineID;
            receiver.migrationList.push_back(migration);
            // update vmObjAddDelDate
            globalCloud->vmObjAddDelDate[machineID][0]=today;
        }
    }
    // update migrationList
    for(int i=0; i<migrateNum && i<mostNum; i++){
        migrateList.erase(migrateList.begin());
    }

    return 0;
}

int RatioStrategy::HandleAdd(Request &req, OneDayResult &receiver) {
    auto& vMachineInfoMap=globalCloud->vmInfoMap; 
    auto model=req.vMachineModel;
    auto InfoIt=vMachineInfoMap.find(model);
    if(InfoIt==vMachineInfoMap.end()){
        LOGE("machine model does not exist");
        exit(-1);
    }
    VMInfo machineInfo= InfoIt->second;
    int vmID=req.vMachineID;
    Deploy res;

    int ratio=globalCloud->VMInfoCalculateRatio(machineInfo);
    auto& serverInfoList=globalCloud->serverInfoList[ratio];
    auto& serverObjList=globalCloud->serverObjList[ratio];
    auto& serverCandidateList=globalCloud->serverCandidateList[ratio];

    // in traversal we find the perfect serverModel1 and serverObjs that we can deploy
    // then we find the min-cost serverObj and get its serverModel2
    // case 1: serverModel1 == serverModel2
    //         perfect case, just deploy
    // case 2: there is no serverObjs that we can deploy
    //         buy a perfect server and deploy it
    // case 3: serverModel1 != serverIModel2
    //         deploy vm on the serverObj of serverModel2, and add it & serverModel1 to the migration list
    int theoryMinCost=1<<30, practiceMinCost=1<<30;
    std::string theoryServerModel, practiceServerModel;
    int ID=-1, ratioID=-1, nodeIndex=-1;
    int theoryCandidateIndex=-1;

    for(int i=0;i<serverCandidateList.size();i++){
        ServerInfo serverInfo=serverCandidateList[i];
        if(serverInfo.canDeployOnSingleNode(machineInfo)||serverInfo.canDeployOnDoubleNode(machineInfo)){
            int cost=globalCloud->computeCost(vmID, machineInfo, serverInfo);
            if(cost<theoryMinCost){
                theoryMinCost=cost;
                serverInfo.getModel(theoryServerModel);
                theoryCandidateIndex=i;
            }
        }
    }

    for(int i=0;i<serverObjList.size();i++){
        auto serverObj=serverObjList[i];
        if(serverObj.canDeployOnSingleNode(0,machineInfo)){
            int cost=globalCloud->computeCost(vmID, machineInfo, serverObj.info);
            if(cost<practiceMinCost){
                practiceMinCost=cost;
                ID=serverObj.ID;
                ratioID=i;
                nodeIndex=0;
                serverObj.getModel(practiceServerModel);
            }
            continue;
        }
        if(serverObj.canDeployOnSingleNode(1,machineInfo)){
            int cost=globalCloud->computeCost(vmID, machineInfo, serverObj.info);
            if(cost<practiceMinCost){
                practiceMinCost=cost;
                ID=serverObj.ID;
                ratioID=i;
                nodeIndex=1;
                serverObj.getModel(practiceServerModel);
            }
            continue;
        }
        if(serverObj.canDeployOnDoubleNode(machineInfo)){
            int cost=globalCloud->computeCost(vmID, machineInfo, serverObj.info);
            if(cost<practiceMinCost){
                practiceMinCost=cost;
                ID=serverObj.ID;
                ratioID=i;
                nodeIndex=2; // doubleNode 
                serverObj.getModel(practiceServerModel);
            }
            continue;
        }
    }

    // case1: perfect case
    if(theoryServerModel.compare(practiceServerModel)==0){
        if(nodeIndex==2){
            globalCloud->addVMObj(serverObjList[ratioID],nodeIndex,model,vmID,ratio);
            res.serverID=ID;
            res.node=-1;
            receiver.deployList.push_back(res);
            return 0;
        }
        else{
            globalCloud->addVMObj(globalCloud->serverObjList[ratio][ratioID],nodeIndex,model,vmID,ratio);
            res.serverID=ID;
            res.node=nodeIndex;
            receiver.deployList.push_back(res);
            return 0;
        }
    }
    // case2: buy a perfect server and deploy it
    else if(ratioID==-1){
        ServerInfo serverInfo=serverCandidateList[theoryCandidateIndex];
        globalCloud->addServerObj(serverInfo);
        receiver.purchaseMap[theoryServerModel]+=1;
        ServerObj& serverObj=serverObjList.back();
        globalCloud->addVMObj(serverObj, 0, model, vmID, ratio);

        res.serverID=serverObj.ID;
        int doubleNode;machineInfo.getDoubleNode(doubleNode);
        if(doubleNode==1){
            res.node=-1;
        }else{
            res.node=0;
        }
        receiver.deployList.push_back(res);
        return 0;
    }
    // case 3: migration case
    else{
        //deploy vm on the serverObj of serverModel2
        if(nodeIndex==2){
            globalCloud->addVMObj(serverObjList[ratioID],nodeIndex,model,vmID,ratio);
            res.serverID=ID;
            res.node=-1;
            receiver.deployList.push_back(res);
        }
        else{
            globalCloud->addVMObj(globalCloud->serverObjList[ratio][ratioID],nodeIndex,model,vmID,ratio);
            res.serverID=ID;
            res.node=nodeIndex;
            receiver.deployList.push_back(res);
        }
        //add vmID & serverModel1 to the migration list
        globalCloud->migrateList[theoryServerModel].push_back(vmID);
        return 0;
    }

    return 0;
}

int RatioStrategy::HandleDel(Request &del, OneDayResult &receiver) {
    int machineId=del.vMachineID;
    globalCloud->delVMObj(machineId);
    return 0;
}
