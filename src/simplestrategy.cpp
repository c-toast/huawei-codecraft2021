//
// Created by ctoast on 2021/3/11.
//

#include "strategy.h"
#include "cloud.h"
#include "utils.h"
#include "cstring"
#include <cmath>
#include <algorithm>

extern SimpleCloud* globalCloud;
ResultRecorder rr;
OldServerDeployer osd;
NewServerDeployer nsd;

std::map<std::string,std::vector<std::string>> fitnessRangeMap; //[VMmodel]range
std::map<std::string,std::map<std::string,int>> fitnessMap; //[VMmodel][serverModel]fitnessRange
int ACCEPT_RANGE=30;
int MIGRATE_RANGE=5;

#define USAGESTATERO 0.8

int Strategy::dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) {
    int dayNum=requestsBunch.dayNum;
    auto& bunch=requestsBunch.bunch;

    nsd.learnModelInfo();

    for(int i=0;i<dayNum;i++){
        OneDayResult oneDayRes;
        OneDayRequest& oneDayReq=bunch[i];

        std::vector<VMObj *> unhandledVMObj;
        std::vector<Request> unhandledDelReqSet;
        std::vector<Request> unhandledAddReqSet;

        osd.deployVMNum=globalCloud->vmObjMap.size();

        for(auto it:oneDayReq){
            if(it.op==ADD){
                auto vmObj=globalCloud->createVMObj(it.vMachineID,it.vMachineModel);
                unhandledVMObj.push_back(vmObj);
                unhandledAddReqSet.push_back(it);
            }else{
                unhandledDelReqSet.push_back(it);
            }
        }
        osd.migrateAndDeploy(unhandledVMObj);
        nsd.buyAndDeploy(unhandledVMObj);
        rr.ouputOneDayRes(unhandledAddReqSet,oneDayRes);

        for(auto it:unhandledDelReqSet){
            HandleDel(it,oneDayRes);
        }
        receiver.push_back(oneDayRes);
    }
    return 0;
}

int Strategy::HandleDel(Request &del, OneDayResult &receiver) {
    int machineId=del.vMachineID;
    globalCloud->delVMObj(machineId);
    return 0;
}

int Strategy::CalFitness(ServerInfo &serverInfo, VMInfo &vmInfo, double &fitnessReceiver) {
    int serverCpuNum=serverInfo.cpuNum;
    int serverMemSize=serverInfo.memorySize;
    int vmCpuNum=vmInfo.cpuNum;
    int vmMemSize=vmInfo.memorySize;

    double cpuNumRadio=((double)vmCpuNum)/serverCpuNum;
    double memSizeRadio=((double)vmMemSize)/serverMemSize;
    double average=(cpuNumRadio+memSizeRadio)/2;
    fitnessReceiver=(pow(cpuNumRadio-average,2)+pow(memSizeRadio-average,2))/2;
    return 0;
}

int Strategy::init() {

    auto serverInfoMap= globalCloud->serverInfoMap;
    auto vmInfoMap=globalCloud->vmInfoMap;
    struct FitRecord{
        std::string serverModel;
        double fitness;
    };

    auto reqSetCmp=[](const FitRecord& r1,const FitRecord& r2){
        return r1.fitness<r2.fitness;
    };

    for(auto &vmInfoMapIt:vmInfoMap){
        std::vector<FitRecord> fitVec;
        for(auto &serverInfoListIt:serverInfoMap){
            FitRecord r;
            r.serverModel=serverInfoListIt.second.model;
            CalFitness(serverInfoListIt.second,vmInfoMapIt.second,r.fitness);
            fitVec.push_back(r);
        }
        std::sort(fitVec.begin(),fitVec.end(),reqSetCmp);
        int i=0;
        for(auto& recordIt:fitVec){
            fitnessRangeMap[vmInfoMapIt.first].push_back(recordIt.serverModel);
            fitnessMap[vmInfoMapIt.first][recordIt.serverModel]=i;
            i++;
        }
    }

    return 0;
}

int OldServerDeployer::migrateAndDeploy(std::vector<VMObj *> &unhandledVMObj) {
    Deploy(unhandledVMObj);
    migrate(unhandledVMObj);
}

int OldServerDeployer::migrate(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpAddReqSet;
    int time=0;
    for(auto vmObj:unhandledVMObj){
        VMInfo vmInfo=vmObj->info;
        bool haveDeploy=false;

        for(auto& it:globalCloud->serverObjList){
            if(!isServerInSD(it,USAGESTATERO)) {
                for (auto includeVMIt:it->vmObjMap) {
                    auto includeVMObj = includeVMIt.second;
                    ServerObj tmpObj = *it;
                    tmpObj.delVM(includeVMObj->id);
                    int deployNode;


                    if (!rr.haveObjDeploy[includeVMObj]
                        && tmpObj.canDeploy(vmInfo, deployNode)
                        && tmpObj.deployVM(deployNode, vmObj)
                        && isMigrateDecisionBetter(it, &tmpObj)
                        && time < (deployVMNum * 5) / 1000) {
                        rr.migrateVMObj(it, includeVMObj);
                        rr.deployVMObj(it->id, deployNode, vmObj);
                        haveDeploy = true;
                        tmpAddReqSet.push_back(includeVMObj);
                        time++;
                        break;
                    }
                }
            }

            if(haveDeploy){
                break;
            }
        }
        if(!haveDeploy){
            tmpAddReqSet.push_back(vmObj);
        }
    }
    unhandledVMObj=tmpAddReqSet;
    return 0;
}

int OldServerDeployer::Deploy(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpAddReqSet;

    for(auto vmObj:unhandledVMObj){
        VMInfo vmInfo=vmObj->info;
        bool haveDeploy=false;
        for(auto& it:globalCloud->serverObjList){
            std::string serverModel=it->info.model;
            int deployNode;
            if(it->canDeploy(vmInfo,deployNode)){
                ServerObj tmpObj=*it;
                tmpObj.deployVM(deployNode,vmObj);
                if(isDeployDecisionBetter(it, &tmpObj)){
                    rr.deployVMObj(it->id, deployNode, vmObj);
                    haveDeploy=true;
                    break;
                }
            }
        }
        if(!haveDeploy){
            tmpAddReqSet.push_back(vmObj);
        }
    }
    unhandledVMObj=tmpAddReqSet;
    return 0;
}

int NewServerDeployer::buyAndDeploy(std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj*> doubleNodeVMObj;
    std::vector<VMObj*> singleNodeVMObj;
    for(auto it:unhandledVMObj){
        if(it->info.doubleNode==1){
            doubleNodeVMObj.push_back(it);
        }
        else{
            singleNodeVMObj.push_back(it);
        }
    }

    std::map<double,std::vector<VMObj*>> classifiedVMObjMap;
    classify(doubleNodeVMObj,classifiedVMObjMap);
    auto Cmp=[](const VMObj* s1,const VMObj* s2){
        return s1->info.cpuNum>s2->info.cpuNum;
    };
    for(int i=0;i<classifiedVMObjMap.size();i++){
        std::sort(classifiedVMObjMap[i].begin(),classifiedVMObjMap[i].end(),Cmp);
    }

    for(auto& it:Clusters){
        std::vector<ServerInfo*>& serverCandidates = it.second;
        std::vector<VMObj*>& vmObjVec=classifiedVMObjMap[it.first];
        int j=0;
        ServerObj serverObj(*serverCandidates[j]);
        //max-min
        for(int i=0;i<vmObjVec.size();i++){
            if(serverObj.canDeployOnDoubleNode(vmObjVec[i]->info)){
                serverObj.deployVM(NODEAB,vmObjVec[i]);
            }else{
                double ful=CalculateFullness(&serverObj);
                if(ful>1.5){
                    globalCloud->deployServerObj(serverObj);
                }
                else{
                    for(j=j+1;j<serverCandidates.size();j++){
                        ServerObj newServerObj(*serverCandidates[j]);
                        if(movVMObjToNewServerObj(&serverObj,&newServerObj)==0){
                            serverObj=newServerObj;
                            break;
                        }
                    }
                    if(j==serverCandidates.size()){
                        j=0;
                        globalCloud->deployServerObj(serverObj);
                        ServerObj serverObj(*serverCandidates[j]);
                    }
                }
                i--;
            }
        }
    }


    auto& serverObjList=globalCloud->serverObjList;
    int oldSize=serverObjList.size();
    for(auto vmObj:unhandledVMObj){
        VMInfo vmInfo=vmObj->info;
        ServerObj* DeployableServerObj=NULL;
        int minFitNessRange=ACCEPT_RANGE;

        for(int i=oldSize;i<serverObjList.size();i++) {
            int deployNode;
            ServerObj* serverObj = serverObjList[i];
            if (serverObj->canDeploy(vmInfo, deployNode)) {
                double fit = fitnessMap[vmInfo.model][serverObj->info.model];
                if (fit <= minFitNessRange) {
                    minFitNessRange = fit;
                    DeployableServerObj = serverObj;
                }
            }
        }

        if(DeployableServerObj!=NULL){
            int deployNode;
            DeployableServerObj->canDeploy(vmInfo,deployNode);
            rr.deployVMObj(DeployableServerObj->id, deployNode, vmObj);
            //globalCloud->deployVMObj(DeployableServerObj->id, deployNode, vmObj);
            continue;
        }

        for(int i=0;;i++) {
            std::string serverModel = fitnessRangeMap[vmInfo.model][i];
            ServerInfo serverInfo=globalCloud->serverInfoMap[serverModel];
            if(!serverInfo.canDeployOnSingleNode(vmInfo)&&!serverInfo.canDeployOnDoubleNode(vmInfo)){
                continue;
            }
            int newId=globalCloud->createServerObj(serverInfo);
            int deployNode;
            globalCloud->serverObjList[newId]->canDeploy(vmInfo,deployNode);
            rr.deployVMObj(newId, deployNode, vmObj);
            //globalCloud->deployVMObj(newId, deployNode, vmObj);

            break;
        }
    }

    return 0;
}

int NewServerDeployer::learnModelInfo() {
    //k-means
    struct infoUnit{
        int type=-1;
        double radio=-1;
        ServerInfo* info;
    };
    std::vector<infoUnit> infoVec;
    for(auto& it:globalCloud->serverInfoMap){
        infoUnit unit;
        unit.info=&it.second;
        unit.radio=(double)it.second.cpuNum/it.second.memorySize;
        infoVec.push_back(unit);
    }
    std::vector<double> ClusterType={0.05,0.3,0.7,1,2,4,7};
    int itTime=30;
    for(int i=0;i<itTime;i++){
        std::vector<double> NewClusterType(ClusterType.size(),0);
        std::vector<double> MemberNum(ClusterType.size(),0);
        for(int j=0;j<infoVec.size();j++){
            double minDis=1000;
            int minDisIndex=-1;
            for(int k=0;k<ClusterType.size();k++){
                double dis=fabs(infoVec[j].radio-ClusterType[k]);
                if(dis<minDis){
                    minDis=dis;
                    minDisIndex=k;
                }
            }
            //LOGI("self radio is %lf, min type is %lf",infoVec[j].radio,ClusterType[minDisIndex]);
            NewClusterType[minDisIndex]+=infoVec[j].radio;
            MemberNum[minDisIndex]+=1;
            infoVec[j].type=minDisIndex;
        }
        for(int j=0;j<ClusterType.size();j++){
            ClusterType[j]=NewClusterType[j]/MemberNum[j];
        }
    }

    for(int i=0;i<infoVec.size();i++){
        double radio=ClusterType[infoVec[i].type];
        Clusters[radio].push_back(infoVec[i].info);
    }

    auto Cmp=[](const ServerInfo* s1,const ServerInfo* s2){
        return s1->cpuNum<s2->cpuNum;
    };
    for(int i=0;i<Clusters.size();i++){
        std::sort(Clusters[i].begin(),Clusters[i].end(),Cmp);
    }

    return 0;
}

int NewServerDeployer::classify(std::vector<VMObj *> &vmObjVec, std::map<double, std::vector<VMObj *>> &receiver) {
    for(int i=0;i<vmObjVec.size();i++){
        double minDis=1000;
        double radio=(double)vmObjVec[i]->info.cpuNum/vmObjVec[i]->info.memorySize;
        double type=-1;
        for(auto& it:Clusters){
            double dis=fabs(it.first-radio);
            if(dis-minDis){
                minDis=dis;
                type=it.first;
            }
        }
        receiver[type].push_back(vmObjVec[i]);
    }
    return 0;
}

//incomplete
int NewServerDeployer::movVMObjToNewServerObj(ServerObj *oldObj, ServerObj *newObj) {
    for(auto it:oldObj->vmObjMap){
        VMObj* vmObj=it.second;
        int deployNode=oldObj->vmObjDeployNodeMap[it.first];
        if(newObj->canDeployOnNode(deployNode,vmObj->info)){
            newObj->deployVM(deployNode,vmObj);
        }else{
            return -1;
        }
    }
    return 0;
}

int ResultRecorder::ouputOneDayRes(std::vector<Request> addReqVec, OneDayResult &receiver) {
    auto objSetCmp=[](const ServerObj* s1,const ServerObj* s2){
        return s1->info.model<s2->info.model;
    };
    auto& serverObjList=globalCloud->serverObjList;
    if(serverObjList.size()>oldSize){
        std::sort(serverObjList.begin()+oldSize, serverObjList.end(), objSetCmp);
    }

    globalCloud->renewServerID(oldSize);

    for(int i=oldSize; i < serverObjList.size();){
        std::string model=serverObjList[i]->info.model;
        Purchase p;
        p.serverName=model;
        p.num=1;
        int j=i+1;
        for(;j<serverObjList.size();j++){
            if(serverObjList[j]->info.model==model){
                p.num++;
            }else{
                break;
            }
        }
        i=j;
        receiver.purchaseVec.push_back(p);
    }

    for(auto& it:addReqVec){
        Deploy d;
        VMObj* obj=globalCloud->vmObjMap[it.vMachineID];
        d.serverID= obj->deployServerID;
        if(obj->info.doubleNode==1){
            d.node=NODEAB;
        }else{
            d.node=obj->deployNodes[0];
        }
        receiver.deployList.push_back(d);
    }


    for(auto& it:migrationVec){
        Migration m;
        m.virtualID=it->id;
        m.serverID=it->deployServerID;
        if(it->info.doubleNode==1){
            m.node=NODEAB;
        }else{
            m.node=it->deployNodes[0];
        }
        receiver.migrationList.push_back(m);
    }

    oldSize=serverObjList.size();
    migrationVec.clear();
    haveObjDeploy.clear();

    return 0;
}

int ResultRecorder::deployVMObj(int serverObjID, int nodeIndex, VMObj *vmObj) {
    globalCloud->deployVMObj(serverObjID,nodeIndex,vmObj);
    haveObjDeploy[vmObj]= true;
    return 0;
}

int ResultRecorder::migrateVMObj(ServerObj *serverObj, VMObj *vmObj) {
    globalCloud->MigrateVMObj(vmObj->id);
    migrationVec.push_back(vmObj);
    return 0;
}


MultiDimension calSingleNodeUsageState(ServerObj* obj, int NodeIndex){
    Resource remainingResource= obj->nodes[NodeIndex].remainingResource;
    int allMem=obj->info.memorySize;
    int allCpu=obj->info.cpuNum;
    MultiDimension ret;
    ret.Dimension1= (double)(allCpu - remainingResource.cpuNum) / allCpu;
    ret.Dimension2= (double)(allMem - remainingResource.memorySize) / allMem;
    return ret;
}

MultiDimension calNodeBalanceState(ServerObj* obj){
    MultiDimension USA=calSingleNodeUsageState(obj, NODEA);
    MultiDimension USB=calSingleNodeUsageState(obj, NODEB);
    MultiDimension ret;
    ret.Dimension1=sqrt(pow(USA.Dimension2, 2) + pow(USA.Dimension1, 2));
    ret.Dimension2=sqrt(pow(USB.Dimension2, 2) + pow(USB.Dimension1, 2));
    return ret;
}

double calDeviation(MultiDimension d){
    double ret;
    ret=d.Dimension1-d.Dimension2;
    if(ret<0){
        ret=-ret;
    }
    ret/=distance(d.Dimension1,d.Dimension2,0,0);
    return ret;
}

bool isInSD(MultiDimension us, double R0){
    double x=us.Dimension1;
    double y=us.Dimension2;
    double dis=distance(x,y,0,0);
    if(dis>=R0||dis<=1-R0){
        return true;
    }
    dis=distance(x,y,R0,1-R0);
    if(dis<=R0){
        return true;
    }
    dis=distance(x,y,1-R0,R0);
    if(dis<=R0){
        return true;
    }
    return false;
}

bool isDeployDecisionBetter(ServerObj *oldServerObj, ServerObj *newServerObj) {
    MultiDimension newNodeAState=calSingleNodeUsageState(newServerObj,NODEA);
    MultiDimension newNodeBState=calSingleNodeUsageState(newServerObj,NODEB);
    MultiDimension oldNodeAState=calSingleNodeUsageState(oldServerObj,NODEA);
    MultiDimension oldNodeBState=calSingleNodeUsageState(oldServerObj,NODEB);
//    bool isNewAInSD=isInSD(newNodeAState,USAGESTATERO);
//    bool isNewBInSD=isInSD(newNodeBState,USAGESTATERO);
//    bool isOldAInSD=isInSD(oldNodeAState,USAGESTATERO);
//    bool isOldBInSD=isInSD(oldNodeBState,USAGESTATERO);

    if(isInSD(newNodeAState,USAGESTATERO)&&isInSD(newNodeBState,USAGESTATERO)){
        return true;
    }

    return false;
}

bool isMigrateDecisionBetter(ServerObj *oldServerObj, ServerObj *newServerObj) {
    MultiDimension newNodeAState=calSingleNodeUsageState(newServerObj,NODEA);
    MultiDimension newNodeBState=calSingleNodeUsageState(newServerObj,NODEB);
    MultiDimension oldNodeAState=calSingleNodeUsageState(oldServerObj,NODEA);
    MultiDimension oldNodeBState=calSingleNodeUsageState(oldServerObj,NODEB);
//    bool isNewAInSD=isInSD(newNodeAState,USAGESTATERO);
//    bool isNewBInSD=isInSD(newNodeBState,USAGESTATERO);
//    bool isOldAInSD=isInSD(oldNodeAState,USAGESTATERO);
//    bool isOldBInSD=isInSD(oldNodeBState,USAGESTATERO);

    if(calDeviation(newNodeAState)<=calDeviation(oldNodeAState)&&
            calDeviation(newNodeBState)<=calDeviation(oldNodeBState)){
        return true;
    }

    return false;
}

bool isServerInSD(ServerObj* serverObj, double R0){
    MultiDimension d1=calSingleNodeUsageState(serverObj,NODEA);
    MultiDimension d2=calSingleNodeUsageState(serverObj,NODEB);
    if(isInSD(d1,R0)&&isInSD(d2,R0)){
        return true;
    }
    return false;
}