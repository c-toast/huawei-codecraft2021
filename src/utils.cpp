//
// Created by ctoast on 2021/3/20.
//

#include <array>
#include "utils.h"
#include "cmath"

FILE* logFile=fopen("/home/ctoast/huawei-codecraft/bin/log2.txt","w");
int cnt=1000;

int Resource::allocResource(Resource &requiredResource) {
    this->cpuNum -= requiredResource.cpuNum;
    this->memorySize -= requiredResource.memorySize;
    if (cpuNum < 0 || memorySize < 0) {
        LOGE("allocation resource exceeds the limit");
        exit(-1);
    }
    return 0;
}

int Resource::freeResource(Resource &requiredResource) {
    this->cpuNum += requiredResource.cpuNum;
    this->memorySize += requiredResource.memorySize;
    return 0;
}

std::array<int, 2> Resource::getResourceArray() {
    return std::array<int, 2>({cpuNum,memorySize});
}


double Resource::CalResourceMagnitude(Resource r) {
    return CalResourceMagnitude(r.cpuNum,r.memorySize);
}

double Resource::CalResourceMagnitude(std::array<int, 2> r) {
    return CalResourceMagnitude(r[0],r[1]);
}

double Resource::CalResourceMagnitude(int cpuNum, int memorySize) {
    double res = CalDistance({(double) cpuNum, (double) memorySize});
    return res;
}

int Resource::isResourceEnough(std::array<int, 2> ownRes, std::array<int, 2> requiredRes) {
    return isResourceEnough(Resource(ownRes),Resource(requiredRes));
}


int Resource::isResourceEnough(Resource ownRes, Resource requiredRes) {
    if(ownRes.cpuNum>=requiredRes.cpuNum&&ownRes.memorySize>=requiredRes.memorySize){
        return true;
    }
    return false;
}




double CalDistance(std::array<double, 2> coordinate1) {
    return sqrt(pow(coordinate1[0], 2) + pow(coordinate1[1], 2));
}

double CalDistance(std::array<double, 2> coordinate1, std::array<double, 2> coordinate2) {
    double res = 0;
    for (int i = 0; i < 2; i++) {
        res += pow(coordinate1[i] - coordinate2[i], 2);
    }
    return sqrt(res);
}


