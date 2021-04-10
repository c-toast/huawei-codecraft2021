//
// Created by ctoast on 2021/3/24.
//

#ifndef HUAWEI_CODECRAFT_GLOBAL_H
#define HUAWEI_CODECRAFT_GLOBAL_H
#include "server.h"
#include "cmath"
#include "cloud.h"
#include "vmdeployer.h"
#include "cloudoperator.h"
#include "newserverbuyer.h"
#include "vmmigrater.h"

extern Cloud* globalCloud;
extern CloudOperator cloudOperator;

extern int totalDay;
extern int globalDay;
extern int totalDayNum,readableDayNum;
extern RequestsBatch requestsBatch;

extern StdReader reader;
extern StdWriter writer;


int initCloud();

int initAllRequest();

int initAll();



#endif //HUAWEI_CODECRAFT_GLOBAL_H
