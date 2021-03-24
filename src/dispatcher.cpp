//
// Created by ctoast on 2021/3/12.
//

#include "dispatcher.h"
#include "global.h"

extern Cloud* globalCloud;

int Dispatcher::run() {
    StdWriter writer;
    ResultList res;
    //currently it directly handle all request
    strategy->dispatch(allRequest,res);
    writer.write(res);

    return 0;
}
