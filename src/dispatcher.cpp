//
// Created by ctoast on 2021/3/12.
//

#include "dispatcher.h"
#include "global.h"

int Dispatcher::run() {
    ResultList res;
    allRequest.clear();
    reader.ReadAllRequests(allRequest);

    totalDay=allRequest.size();
    strategy->serverBuyer->learnModelInfo();

    strategy->serverBuyer->initWhenNewBatchCome();
    strategy->vmMigrater->initWhenNewBatchCome();
    strategy->vmDeployer->initWhenNewBatchCome();
    strategy->dispatch(allRequest,res);
    writer.write(res);

    return 0;
}
