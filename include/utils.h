//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_UTILS_H
#define HUAWEI_CODECRAFT_UTILS_H
#include "iostream"
#include <random>
#include <algorithm>

#define LOGD(...) fprintf(stdout,"D:");fprintf(stdout, __VA_ARGS__); putchar('\n')
#define LOGI(...) fprintf(stdout,"I:");fprintf(stdout, __VA_ARGS__); putchar('\n')
#define LOGE(...) fprintf(stdout,"E:");fprintf(stdout, __VA_ARGS__); putchar('\n')
#define LOGW(...) fprintf(stdout,"W:");fprintf(stdout, __VA_ARGS__); putchar('\n')

#define NODEA 0
#define NODEB 1
#define NODEAB 2
#define SINGLEDEPLOY 0
#define DOUBLEDEPLOY 1

#define MAX_INT 999999
#define Epsilon 0.0001
#define P_l 0.35
#define P_g 0.35
#define Alpha 0.45
#define Tao_0
#define Q_0 0.8
#define NA 10 
//100
#define MAX_ITER 3

//#define MIN(a,b) (a>b? b : a)
//#define MAX(a,b) (a>b? a : b)
//#define ABS(a) (a>0? a : -a)

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
