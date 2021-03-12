#include <iostream>
#include "readwriter.h"
#include "dispatcher.h"

int main() {
    FileReader reader("./simple-test.txt");
    SimpleStrategy strategy;
    Dispatcher dispatcher(&reader,&strategy);
    dispatcher.run();

    return 0;
}
