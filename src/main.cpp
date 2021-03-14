#include <iostream>
#include "readwriter.h"
#include "dispatcher.h"

int main() {
    StdReader reader;
    StdWriter writer;
    //freopen("./simple-test.txt","r",stdin);
    //freopen("./simple-result.txt","w",stdout);
    SimpleStrategy strategy;
    Dispatcher dispatcher(&reader,&writer,&strategy);
    dispatcher.run();
    fflush(stdout);

    return 0;
}
