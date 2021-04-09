//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_UTILS_H
#define HUAWEI_CODECRAFT_UTILS_H
#include "iostream"
#include "vector"
#include "array"

#define LOGD(args...) fprintf(stdout,"D:");fprintf(stdout, ##args); putchar('\n')
#define LOGI(args...) fprintf(stdout,"I:");fprintf(stdout, ##args); putchar('\n')
#define LOGE(args...) fprintf(stdout,"E:");fprintf(stdout, ##args); putchar('\n')
#define LOGW(args...) fprintf(stdout,"W:");fprintf(stdout, ##args); putchar('\n')

extern FILE* logFile;
extern int cnt;
#define FLOGI(args...) if(cnt>0) {fprintf(logFile, ##args); fputc('\n',logFile);cnt--;}

#define NODEA 0
#define NODEB 1
#define NODEAB 2
#define SINGLEDEPLOY 0
#define DOUBLEDEPLOY 1

struct Resource{
    int cpuNum;
    int memorySize;

    Resource()=default;

    Resource(int cpuNum,int memorySize):cpuNum(cpuNum),memorySize(memorySize){};

    Resource(std::array<int,2> res):cpuNum(res[0]),memorySize(res[1]){};

    int allocResource(Resource& requiredResource);

    int freeResource(Resource& requiredResource);

    std::array<int,2> getResourceArray();//return cpuNum memSize

    static int isResourceEnough(Resource ownRes,Resource requiredRes);
    static int isResourceEnough(std::array<int,2> ownRes,std::array<int,2> requiredRes);

    static double CalResourceMagnitude(Resource r);

    static double CalResourceMagnitude(int cpuNum,int memorySize);

    static double CalResourceMagnitude(std::array<int,2> r);
};

double CalDistance(std::array<double,2> coordinate1,std::array<double,2> coordinate2);

double CalDistance(std::array<double,2> coordinate1);



#endif //HUAWEI_CODECRAFT_UTILS_H
