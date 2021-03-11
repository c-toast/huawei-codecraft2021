#include <iostream>
#include "readwriter.h"


int main() {
    FileReader reader("./training-1.txt");
    std::vector<ServerInfo> servers_receiver;
    std::vector<VirtualMachineInfo> vmachine_receiver;
    RequestsBunch bunch;
    reader.ReadServersInfo(servers_receiver);
    reader.ReadVMachineInfo(vmachine_receiver);
    reader.ReadBunchOfRequests(bunch);
    return 0;
}
