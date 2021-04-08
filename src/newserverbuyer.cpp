//
// Created by ctoast on 2021/3/24.
//

#include "newserverbuyer.h"
#include "cloud.h"
#include "algorithm"
#include "cmath"
#include "strategytools.h"
#include "global.h"


int NewServerBuyer::learnModelInfo() {
    for (auto &it:globalCloud->serverInfoMap) {
        allServerInfos.push_back(&it.second);
    }

    //k-means
    struct infoUnit {
        int type = -1;
        double radio = -1;
        ServerInfo *info;
    };
    std::vector<infoUnit> infoVec;
    for (auto &it:globalCloud->serverInfoMap) {
        infoUnit unit;
        unit.info = &it.second;
        unit.radio = (double) it.second.cpuNum / it.second.memorySize;
        infoVec.push_back(unit);
    }
    //std::vector<double> ClusterType={0.05,0.3,0.7,1,2,4,7};
    std::vector<double> ClusterType = {1};
    /*
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
    */
    for (int i = 0; i < infoVec.size(); i++) {
        double radio = ClusterType[infoVec[i].type];
        Clusters[radio].push_back(infoVec[i].info);
    }
    /*
    auto Cmp=[](const ServerInfo* s1,const ServerInfo* s2){
        double costPer1,costPer2;
        costPer1 = 1.0*(s1->hardwareCost+(s1->energyCost*totalDay*0.75)/1.0)/(s1->memorySize+2*s1->cpuNum);
        costPer2 = 1.0*(s2->hardwareCost+(s2->energyCost*totalDay*0.75)/1.0)/(s2->memorySize+2*s2->cpuNum);
        return costPer1<costPer2;
    };
    for(int i=0;i<Clusters.size();i++){
        std::sort(Clusters[ClusterType[i]].begin(),Clusters[ClusterType[i]].end(),Cmp);
    }
    */
    return 0;
}

int NewServerBuyer::classify(std::vector<VMObj *> &vmObjVec, std::map<double, std::vector<VMObj *>> &receiver) {
    for (int i = 0; i < vmObjVec.size(); i++) {
        double minDis = 1000;
        double radio = (double) vmObjVec[i]->info.cpuNum / vmObjVec[i]->info.memorySize;
        double type = -1;
        for (auto &it:Clusters) {
            double dis = fabs(it.first - radio);
            if (dis < minDis) {
                minDis = dis;
                type = it.first;
            }
        }
        receiver[type].push_back(vmObjVec[i]);
    }
    return 0;
}

//incomplete
int NewServerBuyer::movVMObjToNewServerObj(ServerObj *oldObj, ServerObj *newObj) {
    for (auto it:oldObj->vmObjMap) {
        VMObj *vmObj = it.second;
        int deployNode = oldObj->vmObjDeployNodeMap[it.first];
        if (newObj->canDeployOnNode(deployNode, vmObj->info)) {
            cloudOperator.deployVMObjInNewServerObj(newObj, vmObj, deployNode);
        } else {
            return -1;
        }
    }
    return 0;
}

int NewServerBuyer::buyAndDeployDoubleNode(std::vector<VMObj *> &doubleNodeVMObj) {
    std::map<double, std::vector<VMObj *>> classifiedVMObjMap;
    classify(doubleNodeVMObj, classifiedVMObjMap);

//    auto Cmp=[](const VMObj* s1,const VMObj* s2){
//
//        return s1->info.cpuNum+s1->info.memorySize > s2->info.cpuNum+s2->info.memorySize;
//    };
    for (auto &it:classifiedVMObjMap) {
        std::sort(it.second.begin(), it.second.end(), vmObjResMagnitudeCmp);
    }

    for (auto &it:Clusters) {
        std::vector<ServerInfo *> &serverCandidates = it.second;
        std::vector<VMObj *> &vmObjVec = classifiedVMObjMap[it.first];
        std::vector<ServerObj *> tmpServerObjList;
        int num = vmObjVec.size();
        std::vector<ServerObj> finalServerObj(num);
        int startIndex = 0;

        for (int i = 0; i < vmObjVec.size(); i++) {
            double minCost = 1 << 30;
            int minIndex = -1;
            // find on Obj
            for (int j = 0; j < tmpServerObjList.size(); j++) {
                ServerObj *serverObj = tmpServerObjList[j];
                if (serverObj->canDeployOnDoubleNode(vmObjVec[i]->info)) {
                    double tmpCost = 2 * serverObj->nodes[0].remainingResource.cpuNum +
                                     serverObj->nodes[0].remainingResource.memorySize;
                    //double tmpCost=CalCostWithVM(&serverObj->info,vmObjVec[i]->info);
                    if (tmpCost < minCost) {
                        minCost = tmpCost;
                        minIndex = j;
                    }
                }
            }
            if (minIndex != -1) {
                cloudOperator.deployVMObjInNewServerObj(tmpServerObjList[minIndex], vmObjVec[i], NODEAB);
                continue;
            }
            // find on Info
            for (int j = 0; j < serverCandidates.size(); j++) {
                ServerObj serverObj(*serverCandidates[j]);
                if (serverObj.canDeployOnDoubleNode(vmObjVec[i]->info)) {
                    double tmpCost = CalCostWithVM(serverCandidates[j], vmObjVec[i]->info);
                    //double tmpCost=serverObj.nodes[0].remainingResource.cpuNum;
                    if (tmpCost < minCost) {
                        minCost = tmpCost;
                        minIndex = j;
                    }
                }
            }
            ServerObj serverObj(*serverCandidates[minIndex]);
            finalServerObj[startIndex] = serverObj;
            cloudOperator.deployVMObjInNewServerObj(&finalServerObj[startIndex], vmObjVec[i], NODEAB);
            tmpServerObjList.push_back(&finalServerObj[startIndex]);
            startIndex++;
        }
        for (int i = 0; i < tmpServerObjList.size(); i++) {
            cloudOperator.deployNewServerObj(tmpServerObjList[i]);
        }
    }
    return 0;
}

int NewServerBuyer::buyAndDeploySingleNode(std::vector<VMObj *> &singleNodeVMObj) {
    std::map<double, std::vector<VMObj *>> classifiedVMObjMap;
    classify(singleNodeVMObj, classifiedVMObjMap);
    auto Cmp = [](const VMObj *s1, const VMObj *s2) {
        return 2 * s1->info.cpuNum + s1->info.memorySize > 2 * s2->info.cpuNum + s2->info.memorySize;
    };
    for (auto &it:classifiedVMObjMap) {
        std::sort(it.second.begin(), it.second.end(), Cmp);
    }

    for (auto &it:Clusters) {
        std::vector<ServerInfo *> &serverCandidates = it.second;
        std::vector<VMObj *> &vmObjVec = classifiedVMObjMap[it.first];
        std::vector<ServerObj *> tmpServerObjList;
        int num = vmObjVec.size();
        std::vector<ServerObj> finalServerObj(num);
        int startIndex = 0;

        for (int i = 0; i < vmObjVec.size(); i++) {
            double minCost = 1 << 30;
            int minIndex = -1;
            int nodeIndex = -1;
            int fuck = vmObjVec[i]->id;
            // find on Obj
            for (int j = 0; j < tmpServerObjList.size(); j++) {
                ServerObj *serverObj = tmpServerObjList[j];
                /*
                srand((unsigned)time(NULL));
                int node1=rand()%2;
                int node2=(node1==0?1:0);
                */
                if (serverObj->canDeployOnSingleNode(0, vmObjVec[i]->info)) {
                    double tmpCost = 2 * serverObj->nodes[0].remainingResource.cpuNum +
                                     serverObj->nodes[0].remainingResource.memorySize;
                    //double tmpCost=CalCostWithVM(&serverObj->info,vmObjVec[i]->info);
                    if (tmpCost < minCost) {
                        minCost = tmpCost;
                        minIndex = j;
                        nodeIndex = 0;
                    }
                } else if (serverObj->canDeployOnSingleNode(1, vmObjVec[i]->info)) {
                    double tmpCost = 2 * serverObj->nodes[1].remainingResource.cpuNum +
                                     serverObj->nodes[1].remainingResource.memorySize;
                    //double tmpCost=CalCostWithVM(&serverObj->info,vmObjVec[i]->info);
                    if (tmpCost < minCost) {
                        minCost = tmpCost;
                        minIndex = j;
                        nodeIndex = 1;
                    }
                }
            }
            if (minIndex != -1) {
                cloudOperator.deployVMObjInNewServerObj(tmpServerObjList[minIndex], vmObjVec[i], nodeIndex);
                continue;
            }
            // find on Info
            for (int j = 0; j < serverCandidates.size(); j++) {
                ServerObj serverObj(*serverCandidates[j]);
                // for a new server, only check node 0
                if (serverObj.canDeployOnSingleNode(0, vmObjVec[i]->info)) {
                    double tmpCost = CalCostWithVM(serverCandidates[j], vmObjVec[i]->info);
                    if (tmpCost < minCost) {
                        minCost = tmpCost;
                        minIndex = j;
                        nodeIndex = 0;
                    }
                }
            }
            ServerObj serverObj(*serverCandidates[minIndex]);
            finalServerObj[startIndex] = serverObj;
            cloudOperator.deployVMObjInNewServerObj(&finalServerObj[startIndex], vmObjVec[i], 0);
            tmpServerObjList.push_back(&finalServerObj[startIndex]);
            startIndex++;
        }
        for (int i = 0; i < tmpServerObjList.size(); i++) {
            cloudOperator.deployNewServerObj(tmpServerObjList[i]);
        }
    }
    return 0;
}

int NewServerBuyer::initWhenNewBatchCome() {
    return 0;
}


int NewServerBuyer::buyAndDeploy(std::vector<VMObj *> &unhandledVMObj) {
    for (int i = 0; i < unhandledVMObj.size();) {
        std::vector<VMObj *> unhandledBatch;
        int batchSize = 100;
        if (i + batchSize > unhandledVMObj.size()) {
            unhandledBatch.insert(unhandledBatch.end(), unhandledVMObj.begin() + i, unhandledVMObj.end());
            i += unhandledVMObj.size();
        } else {
            unhandledBatch.insert(unhandledBatch.end(), unhandledVMObj.begin() + i,
                                  unhandledVMObj.begin() + i + batchSize);
            i += batchSize;
        }
        while (!unhandledBatch.empty()) {
//            if(globalDay==36){
//                LOGE("size %d", unhandledBatch.size());
//                int a;
//            }

            ServerObj *newServer = createASuitableServer(allServerInfos, unhandledBatch);
            DeployVMInServer(newServer, unhandledBatch);
        }
    }

    return 0;
}

ServerObj *NewServerBuyer::createASuitableServer(std::vector<ServerInfo *> &candidateServers,
                                                 std::vector<VMObj *> &unhandledVMObj) {
    std::vector<double> totalFitnessVec;
    std::vector<double> costVec;
    for (auto infoIt:candidateServers) {
        double totalFitnessRange = 0;
        std::vector<VMObj *> candidateVMObj;
        for (auto it:unhandledVMObj) {
            if (infoIt->canDeployOnSingleNode(it->info) ||
                infoIt->canDeployOnDoubleNode(it->info)){
                candidateVMObj.push_back(it);
            }
        }
        if(candidateVMObj.empty()){
            totalFitnessVec.push_back(-1);
            costVec.push_back(-1);
            continue;;
        }

        for (auto it:unhandledVMObj) {
            double range = fitnessRangeMap[it->info.model][infoIt->model];
            totalFitnessRange += range;
        }
        totalFitnessVec.push_back(totalFitnessRange);

        double cost = 1.0 * (infoIt->hardwareCost + (infoIt->energyCost * totalDay *0.75) / 1.0) /
                      (infoIt->memorySize + 2 * infoIt->cpuNum);
        costVec.push_back(cost);
    }
    normalize(totalFitnessVec);
    normalize(costVec);

    double minValue = 100000000;
    int minIndex = -1;
    for (int i = 0; i < totalFitnessVec.size(); i++) {
        double value = totalFitnessVec[i]/17 + costVec[i];
        if (value < minValue) {
            if (unhandledVMObj.size() == 1 &&
                !candidateServers[i]->canDeployOnSingleNode(unhandledVMObj[0]->info) &&
                !candidateServers[i]->canDeployOnDoubleNode(unhandledVMObj[0]->info)) { ;
            } else {
                minValue = value;
                minIndex = i;
            }

        }
    }
    int id = globalCloud->createServerObj(*candidateServers[minIndex]);

    return globalCloud->serverObjList[id];
}

int NewServerBuyer::DeployVMInServer(ServerObj *newServerObj, std::vector<VMObj *> &unhandledVMObj) {
    std::vector<VMObj *> tmpUnhandledVMObj = unhandledVMObj;
    unhandledVMObj.clear();
    auto Cmp = [newServerObj](const VMObj *v1, const VMObj *v2) {
        double fit1 = fitnessRangeMap[v1->info.model][newServerObj->info.model];
        double fit2 = fitnessRangeMap[v2->info.model][newServerObj->info.model];
        return fit1 < fit2;
    };
    std::sort(tmpUnhandledVMObj.begin(), tmpUnhandledVMObj.end(), Cmp);

    for (auto vmIt:tmpUnhandledVMObj) {
        int nodeIndex = 0;
        if (newServerObj->canDeploy(vmIt->info, nodeIndex)) {
            cloudOperator.deployVMObj(newServerObj->id, nodeIndex, vmIt);
        } else {
            unhandledVMObj.push_back(vmIt);
        }
    }

    return 0;
}

int NewServerBuyer::normalize(std::vector<double> &vec) {
    double sum = 0;
    for (auto it:vec) {
        if(it==-1){
            continue;
        }else{
            sum += it;
        }

    }
    for (auto &it:vec) {
        if(it==-1){
            it=1000;
        }else{
            it = it / sum;
        }

    }
    return 0;
}


