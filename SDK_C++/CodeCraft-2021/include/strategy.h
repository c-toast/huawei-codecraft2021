//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_STRATEGY_H
#define HUAWEI_CODECRAFT_STRATEGY_H

#include <map>
#include <string>
#include "readwriter.h"
#include <cmath>



class Strategy{
public:
    int init();

    int dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver);

    int HandleDel(Request &del, OneDayResult &receiver);

};


#endif //HUAWEI_CODECRAFT_STRATEGY_H
