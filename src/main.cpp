#include <iostream>
#include "dispatcher.h"

int main() {
    freopen("./training-2.txt","r",stdin);
    freopen("./simple-result.txt","w",stdout);
    initAll();
    VMDeployer defaultVMDeployer;
    NewServerBuyer defaultServerBuyer; defaultServerBuyer.learnModelInfo();
    VMMigrater defaultVMMigrater;

    Strategy strategy(&defaultVMDeployer,&defaultServerBuyer,&defaultVMMigrater);
    Dispatcher dispatcher(&strategy);
    dispatcher.run();
    fflush(stdout);
    return 0;
}
