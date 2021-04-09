//
// Created by ctoast on 2021/3/12.
//

#include "dispatcher.h"
#include "global.h"

int Dispatcher::run() {
    int totalDayNum,readableDayNum;
    totalDayNum=reader.ReadTotalDayNum();
    readableDayNum=reader.ReadReadableDayNum();//second round

    strategy->serverBuyer->learnModelInfo();//need to modify
    totalDay=totalDayNum;//need to modify

    //strategy->serverBuyer->init();

    int remainingDayNum=totalDayNum;
    while(remainingDayNum != 0){
        int BatchDayNum= (remainingDayNum > readableDayNum) ? readableDayNum : remainingDayNum;//second round
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
