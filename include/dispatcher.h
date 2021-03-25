//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_DISPATCHER_H
#define HUAWEI_CODECRAFT_DISPATCHER_H

#include "cloud.h"
#include "readwriter.h"
#include "strategy.h"
#include "utils.h"
#include <algorithm>
class Dispatcher{
public:
    RequestReader* reader;

    ResultWriter* writer;

    Strategy* strategy;

    Dispatcher(RequestReader* r,ResultWriter* w,Strategy* s){
        reader=r;
        strategy=s;
        writer=w;
    };

    int run();
};

#endif //HUAWEI_CODECRAFT_DISPATCHER_H
