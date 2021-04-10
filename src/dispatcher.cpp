//
// Created by ctoast on 2021/3/12.
//

#include "dispatcher.h"
#include "global.h"

int Dispatcher::run() {

    totalDayNum=reader.ReadTotalDayNum();
    readableDaylimit=reader.ReadReadableDayNum();//second round

    strategy->serverBuyer->learnModelInfo();//need to modify

    int remainingBatchDayNum=totalDayNum;
    int remainingDayNum=totalDayNum;


    reader.ReadSeveralDaysRequests(readableDaylimit, futureRequestsBatch);
    remainingBatchDayNum-=readableDaylimit;
    RequestsBatch uselessBatch;
    OneDayRequest uselessReq;

    strategy->serverBuyer->initWhenNewDayStart(uselessReq);//need to modify
    strategy->serverBuyer->batchVoteRes=std::vector<double>(strategy->serverBuyer->allServerInfos.size(),0);
    strategy->serverBuyer->initWhenNewBatchCome(futureRequestsBatch,uselessBatch);
    strategy->vmMigrater->initWhenNewBatchCome(futureRequestsBatch);
    strategy->vmDeployer->initWhenNewBatchCome();

    while(remainingDayNum != 0){
        int BatchDayNum= 1;//if more than 2, should have some handle
//        int BatchDayNum=totalDayNum;


        //read current batch
        remainingDayNum-=BatchDayNum;
        RequestsBatch currentRequestsBatch;
        for(int i=0;i<BatchDayNum;i++){
            currentRequestsBatch.push_back(futureRequestsBatch[0]);
        }

        strategy->dispatch(currentRequestsBatch);

        //delete current batch
        for(int i=0;i<BatchDayNum;i++){
            futureRequestsBatch.pop_front();
        }

        //read future batch
        if(remainingBatchDayNum>0){
            remainingBatchDayNum-=BatchDayNum;
            RequestsBatch newFutureBatch;
            reader.ReadSeveralDaysRequests(BatchDayNum, newFutureBatch);
            for(int i=0;i<BatchDayNum;i++){
                futureRequestsBatch.push_back(newFutureBatch[0]);
            }
            strategy->serverBuyer->initWhenNewBatchCome(newFutureBatch,currentRequestsBatch);
            strategy->vmMigrater->initWhenNewBatchCome(futureRequestsBatch);
            strategy->vmDeployer->initWhenNewBatchCome();
        }
    }


    return 0;
}
