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
    std::vector<double> d1=UsageState::calSingleNodeUsageState(serverObj,NODEA);
    std::vector<double> d2=UsageState::calSingleNodeUsageState(serverObj,NODEB);
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

bool isInSD(std::vector<double> vec, double R0){
    double dis1,dis2,dis3;
    dis1=CalDistance(vec,{R0,1-R0});
    dis2=CalDistance(vec,{1-R0,R0});
    dis3=CalDistance(vec,{1,1});
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

bool isInAD(std::vector<double> vec, double r0){
    double dis3;
    dis3=CalDistance(vec,{1,1});
    if(dis3 <= r0){
        return true;
    }
    return false;
}

std::vector<double> UsageState::calSingleNodeUsageState(ServerObj *obj, int NodeIndex) {
    Resource remainingResource= obj->nodes[NodeIndex].remainingResource;
    int allMem=obj->info.memorySize/2;
    int allCpu=obj->info.cpuNum/2;
    std::vector<double> vec(2,0);
    vec[0]= (double)(allCpu - remainingResource.cpuNum) / allCpu;
    vec[1]= (double)(allMem - remainingResource.memorySize) / allMem;
    return vec;
}

bool UsageState::isServerNodeInSD(ServerObj *serverObj, int nodeIndex, double R0) {
    std::vector<double> vec1,vec2;
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
    std::vector<double> vec1,vec2;
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

std::vector<double> BalanceState::calNodeBalanceState(ServerObj *obj) {
    std::vector<double> usageStateA=UsageState::calSingleNodeUsageState(obj, NODEA);
    std::vector<double> usageStateB=UsageState::calSingleNodeUsageState(obj, NODEB);
    std::vector<double> ret(2,0);
    ret[0]=CalDistance(usageStateA);
    ret[1]=CalDistance(usageStateA);
    return ret;
}

bool BalanceState::isServerBalanceInSD(ServerObj *serverObj, int nodeIndex, double R0) {
    std::vector<double> d=BalanceState::calNodeBalanceState(serverObj);
    return isInSD(d,R0);
}

bool BalanceState::isServerBalanceInAD(ServerObj *serverObj, int nodeIndex, double r0) {
    std::vector<double> d=BalanceState::calNodeBalanceState(serverObj);
    return isInAD(d,r0);
}
