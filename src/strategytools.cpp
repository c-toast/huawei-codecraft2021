//
// Created by ctoast on 2021/3/24.
//

#include "strategytools.h"

std::map<std::string,std::vector<std::string>> fitnessRangeMap; //[VMmodel]range
std::map<std::string,std::map<std::string,int>> fitnessMap; //[VMmodel][serverModel]fitnessRange


//double calDeviation(MultiDimension d){
//    double ret;
//    ret=d.Dimension1-d.Dimension2;
//    if(ret<0){
//        ret=-ret;
//    }
//    ret/=distance(d.Dimension1,d.Dimension2,0,0);
//    return ret;
//}



//bool isDeployDecisionBetter(ServerObj *oldServerObj, ServerObj *newServerObj) {
//    MultiDimension newNodeAState=calSingleNodeUsageState(newServerObj,NODEA);
//    MultiDimension newNodeBState=calSingleNodeUsageState(newServerObj,NODEB);
//    MultiDimension oldNodeAState=calSingleNodeUsageState(oldServerObj,NODEA);
//    MultiDimension oldNodeBState=calSingleNodeUsageState(oldServerObj,NODEB);
////    bool isNewAInSD=isInSD(newNodeAState,USAGESTATE_RO);
////    bool isNewBInSD=isInSD(newNodeBState,USAGESTATE_RO);
////    bool isOldAInSD=isInSD(oldNodeAState,USAGESTATE_RO);
////    bool isOldBInSD=isInSD(oldNodeBState,USAGESTATE_RO);
//
//    if(isInSD(newNodeAState, USAGESTATE_RO) && isInSD(newNodeBState, USAGESTATE_RO)){
//        return true;
//    }
//
//    return false;
//}



double CalculateFullness(ServerObj* serverObj){
    std::array<double,2> d1=UsageState::calSingleNodeUsageState(serverObj,NODEA);
    std::array<double,2> d2=UsageState::calSingleNodeUsageState(serverObj,NODEB);
    double max1=(d1[0]>d1[1])?d1[0]:d1[1];
    double max2=(d2[0]>d2[1])?d2[0]:d2[1];
    return max1+max2;
}

int CalFitness(ServerInfo &serverInfo, VMInfo &vmInfo, double &fitnessReceiver) {
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

bool isInSD(std::array<double,2> vec, double R0){
    double dis1,dis2;
    dis1=CalDistance(vec,{R0,1-R0});
    dis2=CalDistance(vec,{1-R0,R0});
    if(dis1<=R0&&dis2<=R0){
        return true;
    }
    if(vec[0]<=1-R0&&vec[1]<=1-R0){
        return true;
    }
    if(vec[0]>=R0&&vec[1]>=R0){
        return true;
    }
    return false;
}

bool isInAD(std::array<double,2> vec, double r0){
    double dis3;
    dis3=CalDistance(vec,{1,1});
    if(dis3 <= r0){
        return true;
    }
    return false;
}

std::array<double,2> UsageState::calSingleNodeUsageState(ServerObj *obj, int NodeIndex) {
    Resource remainingResource= obj->nodes[NodeIndex].remainingResource;
    int allMem=obj->info.memorySize/2;
    int allCpu=obj->info.cpuNum/2;
    std::array<double,2> vec;
    vec[0]= (double)(allCpu - remainingResource.cpuNum) / allCpu;
    vec[1]= (double)(allMem - remainingResource.memorySize) / allMem;
    return vec;
}

bool UsageState::isServerNodeInSD(ServerObj *serverObj, int nodeIndex, double R0) {
    std::array<double,2> vec1,vec2;
    if(nodeIndex==NODEAB){
        vec1=calSingleNodeUsageState(serverObj,NODEA);
        vec2=calSingleNodeUsageState(serverObj,NODEB);
        if(isInSD(vec1,R0)&&isInSD(vec2,R0)){
            return true;
        }
        return false;
    }
    vec1=calSingleNodeUsageState(serverObj,nodeIndex);
    if(isInSD(vec1,R0)){
        return true;
    }
    return false;
}

bool UsageState::isServerNodeInAD(ServerObj *serverObj, int nodeIndex, double R0) {
    std::array<double,2> vec1,vec2;
    if(nodeIndex==NODEAB){
        vec1=calSingleNodeUsageState(serverObj,NODEA);
        vec2=calSingleNodeUsageState(serverObj,NODEB);
        if(isInAD(vec1,R0)&&isInAD(vec2,R0)){
            return true;
        }
        return false;
    }
    vec1=calSingleNodeUsageState(serverObj,nodeIndex);
    if(isInAD(vec1,R0)){
        return true;
    }
    return false;
}

bool UsageState::isServerNodeInASD(ServerObj *serverObj, int nodeIndex, double R0, double r0) {
    if(nodeIndex!=NODEAB){
        return isServerNodeInAD(serverObj,nodeIndex,r0)||isServerNodeInSD(serverObj,nodeIndex,R0);
    }
    bool NodeAInASD=isServerNodeInAD(serverObj,NODEA,r0)||isServerNodeInSD(serverObj,NODEA,R0);
    bool NodeBInASD=isServerNodeInAD(serverObj,NODEB,r0)||isServerNodeInSD(serverObj,NODEB,R0);
    return NodeAInASD&&NodeBInASD;
}

std::array<double,2> BalanceState::calNodeBalanceState(ServerObj *obj) {
    std::array<double,2> usageStateA=UsageState::calSingleNodeUsageState(obj, NODEA);
    std::array<double,2> usageStateB=UsageState::calSingleNodeUsageState(obj, NODEB);
    std::array<double,2> ret;
    ret[0]=CalDistance(usageStateA);
    ret[1]=CalDistance(usageStateB);
    return ret;
}

bool BalanceState::isServerBalanceInSD(ServerObj *serverObj, double R0) {
    std::array<double,2> d=BalanceState::calNodeBalanceState(serverObj);
    return isInSD(d,R0);
}

bool BalanceState::isServerBalanceInAD(ServerObj *serverObj, double r0) {
    std::array<double,2> d=BalanceState::calNodeBalanceState(serverObj);
    return isInAD(d,r0);
}

bool BalanceState::isServerBalanceInASD(ServerObj *serverObj, double R0, double r0) {
    return isServerBalanceInSD(serverObj,R0)||isServerBalanceInAD(serverObj,r0);
}

int vmObjResMagnitudeCmp(const VMObj* vm1,const VMObj* vm2){
    double m1=Resource::CalResourceMagnitude(vm1->info.cpuNum,vm1->info.memorySize);
    double m2=Resource::CalResourceMagnitude(vm2->info.cpuNum,vm2->info.memorySize);
    return m1>m2;
}

double CalCostWithVM(ServerInfo *serverInfo, VMInfo &vmInfo, int vmID){
    int serverCpuNum=serverInfo->cpuNum;
    int serverMemSize=serverInfo->memorySize;
    int hardCost=serverInfo->hardwareCost;
    int energyCost=serverInfo->energyCost;

    int vmCpuNum=vmInfo.cpuNum;
    int vmMemSize=vmInfo.memorySize;
    int day=600;
    //int day=averageDay;
    double cost=(double(vmMemSize)/serverMemSize+2*double(vmCpuNum)/serverCpuNum)*(hardCost+day*energyCost);
    //double cost=(hardCost+day*energyCost)/(2*serverCpuNum+serverMemSize);
    //double cost=(hardCost+day*energyCost)*(2*vmCpuNum+vmMemSize)/(2*serverCpuNum+serverMemSize);
    //double cost1=(hardCost+day*energyCost)/(2*serverCpuNum+serverMemSize);
    //double cost2=(hardCost+day*energyCost)*(2*vmCpuNum+vmMemSize)/(2*serverCpuNum+serverMemSize);
    //double cost=cost1+cost2;
    return cost;
}

double finalScore(ServerInfo *serverInfo, Resource totalResource){
    int serverCpuNum=serverInfo->cpuNum;
    int serverMemSize=serverInfo->memorySize;
    int hardCost=serverInfo->hardwareCost;
    int energyCost=serverInfo->energyCost;
    int vmCpuNum=totalResource.cpuNum;
    int vmMemSize=totalResource.memorySize;

    double cost=(double(vmMemSize)/serverMemSize+2*double(vmCpuNum)/serverCpuNum)*(hardCost+600*energyCost);

    double cpuNumRadio=((double)vmCpuNum)/serverCpuNum;
    double memSizeRadio=((double)vmMemSize)/serverMemSize;
    double average=(cpuNumRadio+memSizeRadio)/2;
    double fitness=(pow(cpuNumRadio-average,2)+pow(memSizeRadio-average,2))/2;

    double score=log(cost)/log(1.0+2.0/(1+fitness));
    return score;
}