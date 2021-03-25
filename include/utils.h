//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_UTILS_H
#define HUAWEI_CODECRAFT_UTILS_H
#include "iostream"
#include "vector"

#define LOGD(args...) fprintf(stdout,"D:");fprintf(stdout, ##args); putchar('\n')
#define LOGI(args...) fprintf(stdout,"I:");fprintf(stdout, ##args); putchar('\n')
#define LOGE(args...) fprintf(stdout,"E:");fprintf(stdout, ##args); putchar('\n')
#define LOGW(args...) fprintf(stdout,"W:");fprintf(stdout, ##args); putchar('\n')

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

    int allocResource(Resource& requiredResource);

    int freeResource(Resource& requiredResource);
};

double CalDistance(std::vector<double> coordinate1,std::vector<double> coordinate2);

double CalDistance(std::vector<double> coordinate1);

double CalResourceMagnitude(Resource r);

double CalResourceMagnitude(int cpuNum,int memorySize);

#endif //HUAWEI_CODECRAFT_UTILS_H
