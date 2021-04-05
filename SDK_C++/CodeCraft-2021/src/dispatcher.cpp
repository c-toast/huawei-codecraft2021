//
// Created by ctoast on 2021/3/12.
//

#include "dispatcher.h"
#include "global.h"

int Dispatcher::run() {
    int totalDayNum,readableDayNum;
    totalDayNum=reader.ReadTotalDayNum();
    readableDayNum=reader.ReadReadableDayNum();

    strategy->serverBuyer->learnModelInfo();//need to modify
    totalDay=requestsBatch.size();//need to modify

    int remainingDayNum=totalDayNum;
    while(remainingDayNum != 0){
        int BatchDayNum= (remainingDayNum > readableDayNum) ? readableDayNum : remainingDayNum;
//        int BatchDayNum=totalDayNum;
        remainingDayNum-=BatchDayNum;

        ResultList res;
        requestsBatch.clear();
        reader.ReadSeveralDaysRequests(BatchDayNum, requestsBatch);

        strategy->serverBuyer->initWhenNewBatchCome();
        strategy->vmMigrater->initWhenNewBatchCome();
        strategy->vmDeployer->initWhenNewBatchCome();
        strategy->dispatch(requestsBatch, res);
        writer.write(res);
        fflush(stdout);
    }


    return 0;
}
