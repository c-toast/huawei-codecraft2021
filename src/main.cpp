#include <iostream>
#include "readwriter.h"
#include "dispatcher.h"

int main() {
    //FileReader reader("./simple-test.txt");
    StdReader reader;
    StdWriter writer;
    freopen("./training-1.txt","r",stdin);
    freopen("./simple-result.txt","w",stdout);
    SimpleStrategy strategy;
    Dispatcher dispatcher(&reader,&writer,&strategy);
    dispatcher.run();

    return 0;
}
