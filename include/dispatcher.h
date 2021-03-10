//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_DISPATCHER_H
#define HUAWEI_CODECRAFT_DISPATCHER_H

#include "cloud.h"
#include "readwriter.h"
#include "strategy.h"

public class Dispatcher{
    Cloud cloud;

    RequestReader reader;

    ResultWriter writer;

    Strategy strategy;
};

#endif //HUAWEI_CODECRAFT_DISPATCHER_H
