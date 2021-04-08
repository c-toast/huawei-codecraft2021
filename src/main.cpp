#include <iostream>
#include "dispatcher.h"
#include "global.h"
#include "cloud.h"

#include "monitor.h"


int main() {
//    reader.file = fopen("/home/ctoast/huawei-codecraft/bin/training-2.txt", "r");
//    writer.file = fopen("/home/ctoast/huawei-codecraft/bin/result.txt", "w");
//    freopen("/home/ctoast/huawei-codecraft/bin/recursive.txt","w",stdout);

    initAll();
    VMDeployer defaultVMDeployer;
    NewServerBuyer defaultServerBuyer;
    VMMigrater defaultVMMigrater;

//    Monitor *m = new Monitor(fopen("/home/ctoast/huawei-codecraft/bin/log.txt", "w"));
//    globalCloud->registerBeforeListener(m);

    Strategy strategy(&defaultVMDeployer, &defaultServerBuyer, &defaultVMMigrater);
    Dispatcher dispatcher(&strategy);
    dispatcher.run();

    return 0;
}
