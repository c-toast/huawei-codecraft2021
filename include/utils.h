//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_UTILS_H
#define HUAWEI_CODECRAFT_UTILS_H
#include "iostream"

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
    Resource()=default;

    Resource(int cpuNum,int memorySize):cpuNum(cpuNum),memorySize(memorySize){};

    int allocResource(Resource& requiredResource){
        this->cpuNum-=requiredResource.cpuNum;
        this->memorySize-=requiredResource.memorySize;
        if(cpuNum<0||memorySize<0){
            LOGE("allocation resource exceeds the limit");
            exit(-1);
        }
    }

    int freeResource(Resource& requiredResource){
        this->cpuNum+=requiredResource.cpuNum;
        this->memorySize+=requiredResource.memorySize;
    }

    int cpuNum;
    int memorySize;
};


#endif //HUAWEI_CODECRAFT_UTILS_H
