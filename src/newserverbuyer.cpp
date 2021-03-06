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

int NewServerBuyer::buyAndDeploy(std::vector<VMObj *> &unhandledVMObj) {
    voteResMap.clear();
//    for (int i = 0; i < unhandledVMObj.size();) {
//        std::vector<VMObj *> unhandledBatch;
//        int batchSize = 100;
//        if (i + batchSize > unhandledVMObj.size()) {
//            unhandledBatch.insert(unhandledBatch.end(), unhandledVMObj.begin() + i, unhandledVMObj.end());
//            i += unhandledVMObj.size();
//        } else {
//            unhandledBatch.insert(unhandledBatch.end(), unhandledVMObj.begin() + i,
//                                  unhandledVMObj.begin() + i + batchSize);
//            i += batchSize;
//        }
//        while (!unhandledBatch.empty()) {
//            ServerObj *newServer = createASuitableServer(allServerInfos, unhandledBatch);
//            DeployVMInServer(newServer, unhandledBatch);
//        }
//    }

    while (!unhandledVMObj.empty()) {
        ServerObj *newServer = createASuitableServer(allServerInfos, unhandledVMObj);
        DeployVMInServer(newServer, unhandledVMObj);
    }

    return 0;
}

int
selectTopElement(std::vector<double> &target, std::vector<double> &receiver, std::vector<int> &indexRecord, int top) {
    receiver = std::vector<double>(top, -1);
    indexRecord = std::vector<int>(top, -1);
    for (int i = 0; i < target.size(); i++) {
        int min = receiver[0];
        int minIndex = 0;
        for (int j = 0; j < receiver.size(); j++) {
            if (receiver[j] < min) {
                min = receiver[j];
                minIndex = j;
            }
        }
        if (target[i] > min) {
            receiver[minIndex] = target[i];
            indexRecord[minIndex] = i;
        }
    }
    return 0;
}

ServerObj *NewServerBuyer::createASuitableServer(std::vector<ServerInfo *> &candidateServers,
                                                 std::vector<VMObj *> &unhandledVMObj) {
    std::vector<double> allVoteRes(candidateServers.size(), 0);
    for (auto it:unhandledVMObj) {
        std::vector<double> voteRes;
        if (voteResMap.find(it) != voteResMap.end()) {
            voteRes = voteResMap[it];
        } else {
            voteForServer(it, candidateServers, voteRes);
        }
        for (int i = 0; i < candidateServers.size(); i++) {
            allVoteRes[i] += voteRes[i];
        }
    }
    std::vector<double> topVoteRes;
    std::vector<int> indexRecord;
    selectTopElement(allVoteRes, topVoteRes, indexRecord, 10);

    int maxIndex = -1;
    double maxVote = -1;
    FLOGI("vote res");
    for (int i = 0; i < topVoteRes.size(); i++) {
        FLOGI("today i res %f, batch res %f", topVoteRes[i], batchVoteRes[indexRecord[i]]);
        double voteRes = topVoteRes[i] + batchVoteRes[indexRecord[i]]/(7*readableDaylimit);
        if (voteRes > maxVote) {
            maxVote = voteRes;
            maxIndex = indexRecord[i];
        }
    }
//    batchVoteRes[maxIndex]-
    int id = globalCloud->createServerObj(*candidateServers[maxIndex]);
    return globalCloud->serverObjList[id];

}

int NewServerBuyer::DeployVMInServer(ServerObj *newServerObj, std::vector<VMObj *> &unhandledVMObj) {
//    std::vector<VMObj *> tmpUnhandledVMObj = unhandledVMObj;
//    unhandledVMObj.clear();
//    auto Cmp = [newServerObj](const VMObj *v1, const VMObj *v2) {
//        double fit1 = fitnessMap[v1->info.model][newServerObj->info.model];
//        double fit2 = fitnessMap[v2->info.model][newServerObj->info.model];
//        return fit1 < fit2;
//    };
//    std::sort(tmpUnhandledVMObj.begin(), tmpUnhandledVMObj.end(), Cmp);
//
//    for (auto vmIt:tmpUnhandledVMObj) {
//        int nodeIndex = 0;
//        if (newServerObj->canDeploy(vmIt->info, nodeIndex)) {
//            cloudOperator.deployVMObj(newServerObj->id, nodeIndex, vmIt);
//        } else {
//            unhandledVMObj.push_back(vmIt);
//        }
//    }

    while (true) {
        int canDeployNum = 0;
        std::vector<VMObj *> tmpUnhandledVMObj = unhandledVMObj;
        unhandledVMObj.clear();
        double minFitness = 10000;
        VMObj *minVMObj = NULL;
        int minNodeIndex = NODEAB;
        for (auto vmIt:tmpUnhandledVMObj) {
            int nodeIndex = 0;
            if (!newServerObj->canDeploy(vmIt->info, nodeIndex)) {
                unhandledVMObj.push_back(vmIt);
                continue;
            }
            canDeployNum++;
            double fitness = 1000;
            if (vmIt->info.doubleNode == 1) {
                fitness = CalFitness(newServerObj, NODEAB, vmIt->info);
            } else {
                if (newServerObj->canDeployOnSingleNode(NODEA, vmIt->info)) {
                    double tmp = CalFitness(newServerObj, NODEA, vmIt->info);
                    if (fitness > tmp) {
                        fitness = tmp;
                        nodeIndex = NODEA;
                    }
                }
                if (newServerObj->canDeployOnSingleNode(NODEB, vmIt->info)) {
                    double tmp = CalFitness(newServerObj, NODEB, vmIt->info);
                    if (fitness > tmp) {
                        fitness = tmp;
                        nodeIndex = NODEB;
                    }
                }
            }
            if (minFitness > fitness) {
                minFitness = fitness;
                if (minVMObj != NULL) {
                    unhandledVMObj.push_back(minVMObj);
                }
                minVMObj = vmIt;
                minNodeIndex = nodeIndex;
            } else {
                unhandledVMObj.push_back(vmIt);
            }
        }
        if (canDeployNum == 0) {
            break;
        }
        cloudOperator.deployVMObj(newServerObj->id, minNodeIndex, minVMObj);
    }


    return 0;
}

int
NewServerBuyer::voteForServer(VMObj *vmObj, std::vector<ServerInfo *> &candidateServers, std::vector<double> &voteRes) {
    voteForServer(vmObj->info, candidateServers, voteRes);
    voteResMap[vmObj] = voteRes;
    return 0;
}

int NewServerBuyer::init() {
    return 0;
}


int NewServerBuyer::initWhenNewDayStart(OneDayRequest &currentDayReq) {
    for (auto &it:globalCloud->serverInfoMap) {
        ServerInfo &infoIt = it.second;
        double unitCost = 1.0 * (infoIt.hardwareCost + (infoIt.energyCost * (totalDayNum - globalDay) *1.0 / 1.0)) /
                          (CalDistance({(double) infoIt.memorySize, 2 * (double) infoIt.cpuNum}));
        it.second.unitCost = unitCost;
    }

    for (auto it:currentDayReq) {
        if (it.op == ADD) {
            auto voteRes=batchVoteResMap.find(it.vmID);
            if(voteRes==batchVoteResMap.end()){
                LOGE("NewServerBuyer::initWhenNewDayStart");
                exit(-1);
            }
            for (int i = 0; i < allServerInfos.size(); i++) {
                batchVoteRes[i] -= (voteRes->second)[i];
            }
        }
    }


    return 0;
}

int NewServerBuyer::initWhenNewBatchCome(RequestsBatch &newBatch, RequestsBatch &oldBatch) {

//    batchVoteRes = std::vector<double>(allServerInfos.size(), 0);
    std::vector<VMInfo> unhandledVMInfo;
    for (auto &oneDayReq:newBatch) {
        for (auto it:oneDayReq) {
            if (it.op == ADD) {
                VMInfo info = globalCloud->vmInfoMap[it.vMachineModel];
                std::vector<double> voteRes;
                voteForServer(info, allServerInfos, voteRes);
                batchVoteResMap[it.vmID]=voteRes;
                for (int i = 0; i < allServerInfos.size(); i++) {
                    batchVoteRes[i] += voteRes[i];
                }
            }
        }
    }

    return 0;
}

int NewServerBuyer::voteForServer(VMInfo vmInfo, std::vector<ServerInfo *> &candidateServers,
                                  std::vector<double> &voteRes) {
    voteRes = std::vector<double>(candidateServers.size(), 0);
    std::vector<double> tickets;
    double ticketsValue = 10;

    for (int i = 0; i < 50; i++) {
        tickets.push_back(ticketsValue);
        ticketsValue -= 0.2;
    }

    struct unitCost {
        double value = 1 << 20;
        int index = -1;
    };

    std::vector<unitCost> unitCostVec;
    for (int i = 0; i < voteRes.size(); i++) {
        unitCost u;
        if (candidateServers[i]->canDeployOnSingleNode(vmInfo) ||
            candidateServers[i]->canDeployOnDoubleNode(vmInfo)) {
            double fitness = fitnessMap[vmInfo.model][candidateServers[i]->model];
            double value = candidateServers[i]->unitCost * ((fitness-1)*2+1);
            u.value = value;
            u.index = i;
        } else {
            u.value = 1 << 20;
            u.index = -1;
        }
        unitCostVec.push_back(u);
    }
    auto cmp = [](unitCost &u1, unitCost &u2) {
        return u1.value < u2.value;
    };

    std::sort(unitCostVec.begin(), unitCostVec.end(), cmp);
    Resource r;
    vmInfo.getRequiredResourceForOneNode(r);
    double mag = Resource::CalResourceMagnitude(r);
//        double mag =CalDistance({(double)vmInfo.memorySize, 2 * (double)vmInfo.cpuNum});
    mag = sqrt(mag);

    for (int i = 0; i < tickets.size(); i++) {
        if (unitCostVec[i].index == -1) {
            break;
        }
        double ticket = tickets[i] * mag;
        voteRes[unitCostVec[i].index] = ticket;
    }


    return 0;
}


