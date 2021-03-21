#include <iostream>
#include "readwriter.h"
#include "dispatcher.h"

int main() {
    StdReader reader;
    StdWriter writer;
    //freopen("./training-1.txt","r",stdin);
    //freopen("./simple-result.txt","w",stdout);
    Strategy strategy;
    Dispatcher dispatcher(&reader,&writer,&strategy);
    dispatcher.run();
    fflush(stdout);

    return 0;
}
