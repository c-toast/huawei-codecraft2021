//
// Created by ctoast on 2021/3/24.
//

#include "newserverbuyer.h"
#include "cloud.h"
#include "algorithm"
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
        std::sort(Clusters[ClusterType[i]].begin(),Clusters[ClusterType[i]].end(),Cmp);
    }

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
    auto Cmp=[](const VMObj* s1,const VMObj* s2){
        return s1->info.cpuNum>s2->info.cpuNum;
    };
    for(auto &it:classifiedVMObjMap){
        std::sort(it.second.begin(),it.second.end(),Cmp);
    }

    for(auto& it:Clusters){
        std::vector<ServerInfo*>& serverCandidates = it.second;
        std::vector<VMObj*>& vmObjVec=classifiedVMObjMap[it.first];
        int j=0;
        ServerObj serverObj(*serverCandidates[j]);
        for(int i=0;i<vmObjVec.size();i++){
            if(serverObj.canDeployOnDoubleNode(vmObjVec[i]->info)){
                //serverObj.deployVM(NODEAB,vmObjVec[i]);
                cloudOperator.deployVMObjInNewServerObj(&serverObj, vmObjVec[i], NODEAB);
            }else{
                double ful=CalculateFullness(&serverObj);
                if(ful>1.5){
                    j=0;
                    cloudOperator.deployNewServerObj(&serverObj);
                    //globalCloud->deployServerObj(serverObj);
                    serverObj=ServerObj(*serverCandidates[j]);
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
                        cloudOperator.deployNewServerObj(&serverObj);
                        serverObj=ServerObj(*serverCandidates[j]);
                    }
                }
                i--;
            }
        }

        if(serverObj.vmObjMap.size()>0){
            cloudOperator.deployNewServerObj(&serverObj);
        }
    }


    return 0;
}

int NewServerBuyer::buyAndDeploySingleNode(std::vector<VMObj *> &singleNodeVMObj) {
    std::map<double,std::vector<VMObj*>> classifiedVMObjMap;
    classify(singleNodeVMObj, classifiedVMObjMap);
    auto Cmp=[](const VMObj* s1,const VMObj* s2){
        return s1->info.cpuNum>s2->info.cpuNum;
    };
    for(auto &it:classifiedVMObjMap){
        std::sort(it.second.begin(),it.second.end(),Cmp);
    }

    for(auto& it:Clusters){
        std::vector<ServerInfo*>& serverCandidates = it.second;
        std::vector<VMObj*>& vmObjVec=classifiedVMObjMap[it.first];
        int j=0;
        ServerObj serverObj(*serverCandidates[j]);
        int nodeIndex=0;

        for(int i=0;i<vmObjVec.size();i++){
            if(serverObj.canDeployOnSingleNode(nodeIndex,vmObjVec[i]->info)){
                //serverObj.deployVM(nodeIndex,vmObjVec[i]);
                cloudOperator.deployVMObjInNewServerObj(&serverObj, vmObjVec[i], nodeIndex);
                nodeIndex=(nodeIndex+1)%2;
            }else if(serverObj.canDeployOnSingleNode(nodeIndex+1,vmObjVec[i]->info)){
                //serverObj.deployVM(nodeIndex,vmObjVec[i]);
                cloudOperator.deployVMObjInNewServerObj(&serverObj, vmObjVec[i], nodeIndex + 1);
            }
            else{
                double ful=CalculateFullness(&serverObj);
                if(ful>1.5){
                    j=0;
                    cloudOperator.deployNewServerObj(&serverObj);
                    serverObj=ServerObj(*serverCandidates[j]);
                }
                else{
                    for(j=j+1;j<serverCandidates.size();j++){
                        ServerObj newServerObj(*serverCandidates[j]);
                        if(movVMObjToNewServerObj(&serverObj,&newServerObj)==0){
                            serverObj=newServerObj;
                            break;
                        }
                    }
                    if(j>=serverCandidates.size()){
                        j=0;
                        cloudOperator.deployNewServerObj(&serverObj);
                        serverObj=ServerObj(*serverCandidates[j]);
                    }
                }
                i--;
            }
        }


        if(serverObj.vmObjMap.size()>0){
            cloudOperator.deployNewServerObj(&serverObj);
        }

    }
    return 0;
}

