#include <iostream>
#include "dispatcher.h"
#include "global.h"

int main() {
    reader.file=fopen("./training-1.txt","r");
    writer.file=fopen("./result.txt","w");

    initAll();
    VMDeployer defaultVMDeployer;
    NewServerBuyer defaultServerBuyer;
    VMMigrater defaultVMMigrater;

    Strategy strategy(&defaultVMDeployer,&defaultServerBuyer,&defaultVMMigrater);
    Dispatcher dispatcher(&strategy);
    dispatcher.run();

    return 0;
}
