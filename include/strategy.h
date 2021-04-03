//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_STRATEGY_H
#define HUAWEI_CODECRAFT_STRATEGY_H

#include <map>
#include <string>
#include "readwriter.h"
#include "global.h"
#include <cmath>

class Strategy{
public:
    Strategy()=delete;

    Strategy(VMDeployer* vmDeployer, NewServerBuyer* serverBuyer,VMMigrater* vmMigrater):
    vmDeployer(vmDeployer), serverBuyer(serverBuyer), vmMigrater(vmMigrater){};

    int dispatch(RequestsBatch &batch, std::vector<OneDayResult> &receiver);

public:
    VMDeployer* vmDeployer;
    NewServerBuyer* serverBuyer;
    VMMigrater* vmMigrater;
};


#endif //HUAWEI_CODECRAFT_STRATEGY_H
