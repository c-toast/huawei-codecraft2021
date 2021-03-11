//
// Created by ctoast on 2021/3/10.
//

#ifndef HUAWEI_CODECRAFT_STRATEGY_H
#define HUAWEI_CODECRAFT_STRATEGY_H

#include "readwriter.h"
class Strategy{
    virtual int dispatch(RequestsBunch &requestsBunch, Result &receiver) =0;
};

class SimpleStrategy: public Strategy{

    int dispatch(RequestsBunch &requestsBunch, Result &receiver) override;

    int HandleAdd(Request &req, Result &receiver);

    int HandleDel(Request &del, Result &receiver);
};



#endif //HUAWEI_CODECRAFT_STRATEGY_H
