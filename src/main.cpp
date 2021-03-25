#include <iostream>
#include "readwriter.h"
#include "dispatcher.h"
#include "ant_colony.h"

int main() {
    StdReader reader;
    StdWriter writer;
    freopen("C:\\Users\\12709\\Documents\\GitHub\\huawei-codecraft2021\\src\\training-2.txt","r",stdin);
    freopen("C:\\Users\\12709\\Documents\\GitHub\\huawei-codecraft2021\\src\\result-2.txt","w",stdout);
    //SimpleStrategy strategy;
	Ant_colony ant_colony;
    Dispatcher dispatcher(&reader,&writer,&ant_colony);
    dispatcher.run();
    fflush(stdout);

    return 0;
}
