//
// Created by ctoast on 2021/3/24.
//

#include "vmmigrater.h"
#include "global.h"

int VMMigrater::migrate(std::vector<VMObj *> &unhandledVMObj) {
    int time=0;
    if(time>= (deployVMNum * 5) / 1000){
        return 0;
    }
    for(auto serverIt:globalCloud->serverObjList){
        int counter=0;
        ServerObj tmpserverIt=cloudOperator.getFakeServerObj(serverIt);
        for(; !isServerInSD(&tmpserverIt, USAGESTATERO) && counter < 20;) {
            for (auto vmMapIt:tmpserverIt.vmObjMap) {
                counter++;
                std::string vmModel = vmMapIt.second->info.model;
                int range = fitnessMap[vmModel][tmpserverIt.info.model];
                if (range > ACCEPT_RANGE) {
                    cloudOperator.markMigratedVMObj(&tmpserverIt, vmMapIt.second);
                    cloudOperator.delVMObjInFakeServerObj(&tmpserverIt, vmMapIt.second->id);
                    unhandledVMObj.push_back(vmMapIt.second);
                    time++;
                    break;
                }
            }
            if (time >= (deployVMNum * 5) / 1000) {
                return 0;
            }

            for (; !isServerNodeBalance(&tmpserverIt, NODEBANLANCESTATERO);) {
                Resource rA, rB;
                tmpserverIt.getNodeRemainingResource(NODEA, rA);
                tmpserverIt.getNodeRemainingResource(NODEB, rB);
                int nodeIndex;
                if (distance(rA.cpuNum, rA.memorySize, 0, 0) > distance(rB.cpuNum, rB.memorySize, 0, 0)) {
                    nodeIndex = NODEB;
                } else {
                    nodeIndex = NODEA;
                }
                for (auto vmMapIt:tmpserverIt.vmObjMap) {
                    VMObj *vmObj = vmMapIt.second;
                    if (vmObj->info.doubleNode != 1 && vmObj->deployNodes[0] == nodeIndex) {
                        cloudOperator.markMigratedVMObj(&tmpserverIt, vmMapIt.second);
                        cloudOperator.delVMObjInFakeServerObj(&tmpserverIt, vmMapIt.second->id);
                        unhandledVMObj.push_back(vmMapIt.second);
                        time++;
                        break;
                    }
                }
                if (time >= (deployVMNum * 5) / 1000) {
                    return 0;
                }
            }

        }
    }

    return 0;
}

