#include <iostream>
#include "readwriter.h"


int main() {
    FileReader reader("./training-1.txt");
    std::vector<ServerInfo> receiver;
    reader.ReadServersInfo(receiver);
    return 0;
}
