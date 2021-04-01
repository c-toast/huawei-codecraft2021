//
// Created by ctoast on 2021/3/20.
//

#include <array>
#include "utils.h"
#include "cmath"

int Resource::allocResource(Resource& requiredResource){
    this->cpuNum-=requiredResource.cpuNum;
    this->memorySize-=requiredResource.memorySize;
    if(cpuNum<0||memorySize<0){
        LOGE("allocation resource exceeds the limit");
        exit(-1);
    }
    return 0;
}

int Resource::freeResource(Resource& requiredResource){
    this->cpuNum+=requiredResource.cpuNum;
    this->memorySize+=requiredResource.memorySize;
    return 0;
}

//double CalDistance(std::vector<double> coordinate1, std::vector<double> coordinate2) {
//    double res=0;
//    for(int i=0;i<coordinate1.size();i++){
//        res+=pow(coordinate1[i]-coordinate2[i],2);
//    }
//    return sqrt(res);
//}
//
//double CalDistance(std::vector<double> coordinate1) {
//    std::vector<double> coordinate2(coordinate1.size(),0);
//    return CalDistance(coordinate1,coordinate2);
//}

double CalDistance(std::array<double,2> coordinate1) {
    return sqrt(pow(coordinate1[0],2)+pow(coordinate1[1],2));
}

double CalResourceMagnitude(Resource r) {
    double res=CalDistance({(double)r.cpuNum,(double)r.memorySize},{0,0});
    return res;
}

double CalResourceMagnitude(int cpuNum, int memorySize) {
    double res=CalDistance({(double)cpuNum,(double)memorySize},{0,0});
    return res;
}

double CalDistance(std::array<double, 2> coordinate1, std::array<double, 2> coordinate2) {
    double res=0;
    for(int i=0;i<2;i++){
        res+=pow(coordinate1[i]-coordinate2[i],2);
    }
    return sqrt(res);
}