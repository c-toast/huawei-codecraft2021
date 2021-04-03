#include <iostream>
#include "dispatcher.h"
#include "global.h"

int main() {
//    reader.file=fopen("/home/ctoast/huawei-codecraft/bin/training-1.txt","r");
//    writer.file=fopen("/home/ctoast/huawei-codecraft/bin/result.txt","w");

    initAll();
    VMDeployer defaultVMDeployer;
    NewServerBuyer defaultServerBuyer;
    VMMigrater defaultVMMigrater;

    Strategy strategy(&defaultVMDeployer,&defaultServerBuyer,&defaultVMMigrater);
    Dispatcher dispatcher(&strategy);
    dispatcher.run();

    return 0;
}
