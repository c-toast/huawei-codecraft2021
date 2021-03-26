#include <iostream>
#include "dispatcher.h"
#include <fstream>

int main() {
    //StdWriter::stream=&std::cout; StdReader::file=stdin;
//    std::ofstream stream("/home/ctoast/huawei-codecraft/cmake-build-debug/simple-result.txt",std::fstream::out);
//    StdWriter::stream=&stream;
//    StdReader::file=fopen("/home/ctoast/huawei-codecraft/cmake-build-debug/training-1.txt","r");

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
