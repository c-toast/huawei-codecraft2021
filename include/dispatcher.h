//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_DISPATCHER_H
#define HUAWEI_CODECRAFT_DISPATCHER_H

#include "cloud.h"
#include "readwriter.h"
#include "strategy.h"

class Dispatcher{
public:

    Strategy* strategy;

    Dispatcher(Strategy* s){
        strategy=s;
    };

    int run();
};

#endif //HUAWEI_CODECRAFT_DISPATCHER_H
