//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_UTILS_H
#define HUAWEI_CODECRAFT_UTILS_H
#include "iostream"

#define LOGD(...) fprintf(stdout,"D:");fprintf(stdout, __VA_ARGS__); putchar('\n')
#define LOGI(...) fprintf(stdout,"I:");fprintf(stdout, __VA_ARGS__); putchar('\n')
#define LOGE(...) fprintf(stdout,"E:");fprintf(stdout, __VA_ARGS__); putchar('\n')
#define LOGW(...) fprintf(stdout,"W:");fprintf(stdout, __VA_ARGS__); putchar('\n')

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
        return 0;
    }

    int freeResource(Resource& requiredResource){
        this->cpuNum+=requiredResource.cpuNum;
        this->memorySize+=requiredResource.memorySize;
        return 0;
    }

    int cpuNum;
    int memorySize;
};




#endif //HUAWEI_CODECRAFT_UTILS_H
