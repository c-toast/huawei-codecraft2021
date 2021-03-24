//
// Created by ctoast on 2021/3/24.
//

#include "strategytools.h"


//the type should be Cloud*
SimpleCloud* globalCloud=new SimpleCloud();
cloudoperator rr;
VMDeployer osd;
NewServerDeployer nsd;

std::map<std::string,std::vector<std::string>> fitnessRangeMap; //[VMmodel]range
std::map<std::string,std::map<std::string,int>> fitnessMap; //[VMmodel][serverModel]fitnessRange
int ACCEPT_RANGE=10;

MultiDimension calSingleNodeUsageState(ServerObj* obj, int NodeIndex){
    Resource remainingResource= obj->nodes[NodeIndex].remainingResource;
    int allMem=obj->info.memorySize/2;
    int allCpu=obj->info.cpuNum/2;
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

bool isServerNodeBalance(ServerObj* serverObj,double R0){
    MultiDimension d=calNodeBalanceState(serverObj);
    return isInSD(d,R0);
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
    double dis1,dis2,dis3;
    dis1=distance(x,y,R0,1-R0);
    dis2=distance(x,y,1-R0,R0);
    dis3=distance(x,y,1,1);
    if(dis3<=USAGESTATErO){
        return true;
    }
    if(dis1<=R0&&dis2<=R0){
        return true;
    }

    if(x<=1-R0&&y<=1-R0){
        return true;
    }
    if(x>=R0&&y>=R0){
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

double CalculateFullness(ServerObj* serverObj){
    MultiDimension d1=calSingleNodeUsageState(serverObj,NODEA);
    MultiDimension d2=calSingleNodeUsageState(serverObj,NODEA);
    double max1=(d1.Dimension1>d1.Dimension2)?d1.Dimension1:d1.Dimension2;
    double max2=(d2.Dimension1>d2.Dimension2)?d2.Dimension1:d2.Dimension2;
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