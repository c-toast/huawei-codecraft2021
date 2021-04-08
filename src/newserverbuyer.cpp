//
// Created by ctoast on 2021/3/24.
//

#include "newserverbuyer.h"
#include "cloud.h"
#include "algorithm"
#include "ctime"
#include "cmath"
#include "strategytools.h"
#include "global.h"

int NewServerBuyer::buyAndDeploy(std::vector<VMObj *> &unhandledVMObj) {
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
    buyAndDeployDoubleNode(doubleNodeVMObj);
    buyAndDeploySingleNode(singleNodeVMObj);

    return 0;
}

int NewServerBuyer::learnModelInfo() {
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
    //std::vector<double> ClusterType={0.05,0.3,0.7,1,2,4,7};
    std::vector<double> ClusterType={0.05,0.5,1,5};
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

    /*
    auto Cmp=[](const ServerInfo* s1,const ServerInfo* s2){
        double costPer1,costPer2;
        //costPer1 = 1.0*(s1->hardwareCost+(s1->energyCost*totalDay*0.75)/1.0)/(s1->memorySize+2*s1->cpuNum);
        //costPer2 = 1.0*(s2->hardwareCost+(s2->energyCost*totalDay*0.75)/1.0)/(s2->memorySize+2*s2->cpuNum);
        costPer1 = (s1->memorySize+s1->cpuNum);
        costPer2 = (s2->memorySize+s2->cpuNum);
        return costPer1<costPer2;
    };
    for(int i=0;i<Clusters.size();i++){
        std::sort(Clusters[ClusterType[i]].begin(),Clusters[ClusterType[i]].end(),Cmp);
    }
    */

    return 0;
}

int NewServerBuyer::classify(std::vector<VMObj *> &vmObjVec, std::map<double, std::vector<VMObj *>> &receiver) {
    for(int i=0;i<vmObjVec.size();i++){
        double minDis=1000;
        double radio=(double)vmObjVec[i]->info.cpuNum/vmObjVec[i]->info.memorySize;
        double type=-1;
        for(auto& it:Clusters){
            double dis=fabs(it.first-radio);
            if(dis<minDis){
                minDis=dis;
                type=it.first;
            }
        }
        receiver[type].push_back(vmObjVec[i]);
    }
    return 0;
}

//incomplete
int NewServerBuyer::movVMObjToNewServerObj(ServerObj *oldObj, ServerObj *newObj) {
    for(auto it:oldObj->vmObjMap){
        VMObj* vmObj=it.second;
        int deployNode=oldObj->vmObjDeployNodeMap[it.first];
        if(newObj->canDeployOnNode(deployNode,vmObj->info)){
            cloudOperator.deployVMObjInNewServerObj(newObj,vmObj,deployNode);
        }else{
            return -1;
        }
    }
    return 0;
}

int NewServerBuyer::buyAndDeployDoubleNode(std::vector<VMObj *> &doubleNodeVMObj) {
    std::map<double,std::vector<VMObj*>> classifiedVMObjMap;
    classify(doubleNodeVMObj,classifiedVMObjMap);
    /*
    auto Cmp=[](const VMObj* s1,const VMObj* s2){
        return s1->info.cpuNum+s1->info.memorySize > s2->info.cpuNum+s2->info.memorySize;
    };
    for(auto &it:classifiedVMObjMap){
        std::sort(it.second.begin(),it.second.end(),Cmp);
    }
    */
    for(auto& it:Clusters){
        std::vector<ServerInfo*>& serverCandidates = it.second;
        std::vector<VMObj*>& vmObjVec=classifiedVMObjMap[it.first];
        int totalNum=vmObjVec.size();
        int startIndex=0;

        Resource totalResource(0,0);
        // calculate total resource
        for(int i=0;i<totalNum;i++){
            Resource tmpResource(vmObjVec[i]->info.cpuNum,vmObjVec[i]->info.memorySize);
            totalResource.freeResource(tmpResource);
        }

        while(startIndex<totalNum){
            // find best server
            double minCost=1<<30;
            int minIndex=-1;
            for(int j=0;j<serverCandidates.size();j++){
                ServerObj serverObj(*serverCandidates[j]);
                int tmpCost=finalScore(&serverObj.info, totalResource);
                if(tmpCost<minCost){
                    minCost=tmpCost;
                    minIndex=j;
                }
            }
            ServerObj serverObj(*serverCandidates[minIndex]);
            // deploy
            while(startIndex<totalNum && serverObj.canDeployOnDoubleNode(vmObjVec[startIndex]->info)){
                cloudOperator.deployVMObjInNewServerObj(&serverObj, vmObjVec[startIndex], NODEAB);
                Resource tmpResource(vmObjVec[startIndex]->info.cpuNum,vmObjVec[startIndex]->info.memorySize);
                totalResource.allocResource(tmpResource);
                startIndex++;
            }
            cloudOperator.deployNewServerObj(&serverObj);
        }
    }
    return 0;
}

int NewServerBuyer::buyAndDeploySingleNode(std::vector<VMObj *> &singleNodeVMObj) {
    std::map<double,std::vector<VMObj*>> classifiedVMObjMap;
    classify(singleNodeVMObj, classifiedVMObjMap);
    /*
    auto Cmp=[](const VMObj* s1,const VMObj* s2){
        return s1->info.cpuNum > s2->info.cpuNum;
    };
    for(auto &it:classifiedVMObjMap){
        std::sort(it.second.begin(),it.second.end(),Cmp);
    }
    */
    for(auto& it:Clusters){
        std::vector<ServerInfo*>& serverCandidates = it.second;
        std::vector<VMObj*>& vmObjVec=classifiedVMObjMap[it.first];
        int totalNum=vmObjVec.size();
        int startIndex=0;

        Resource totalResource(0,0);
        // calculate total resource
        for(int i=0;i<totalNum;i++){
            Resource tmpResource(vmObjVec[i]->info.cpuNum,vmObjVec[i]->info.memorySize);
            totalResource.freeResource(tmpResource);
        }

        while(startIndex<totalNum){
            // find best server
            double minCost=1<<30;
            int minIndex=-1;
            for(int j=0;j<serverCandidates.size();j++){
                ServerObj serverObj(*serverCandidates[j]);
                int tmpCost=finalScore(&serverObj.info, totalResource);
                if(tmpCost<minCost){
                    minCost=tmpCost;
                    minIndex=j;
                }
            }
            ServerObj serverObj(*serverCandidates[minIndex]);
            // deploy
            while(startIndex<totalNum){
                srand((unsigned)time(NULL));
                int node1=rand()%2;
                int node2=(node1==0?1:0);
                if(serverObj.canDeployOnSingleNode(node1,vmObjVec[startIndex]->info)){
                    cloudOperator.deployVMObjInNewServerObj(&serverObj, vmObjVec[startIndex], node1);
                    Resource tmpResource(vmObjVec[startIndex]->info.cpuNum,vmObjVec[startIndex]->info.memorySize);
                    totalResource.allocResource(tmpResource);
                    startIndex++;
                }
                else if(serverObj.canDeployOnSingleNode(node2,vmObjVec[startIndex]->info)){
                    cloudOperator.deployVMObjInNewServerObj(&serverObj, vmObjVec[startIndex], node2);
                    Resource tmpResource(vmObjVec[startIndex]->info.cpuNum,vmObjVec[startIndex]->info.memorySize);
                    totalResource.allocResource(tmpResource);
                    startIndex++;
                }
                else break;
            }
            cloudOperator.deployNewServerObj(&serverObj);
        }
    }
    return 0;
}

int NewServerBuyer::initWhenNewBatchCome() {
    return 0;
}


