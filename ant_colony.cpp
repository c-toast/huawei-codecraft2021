#include "strategy.h"
#include "cloud.h"
#include "ant_colony.h"
#include "utils.h"
#include "cstring"

extern SimpleCloud* globalCloud;

ServerInfo binarySearch(std::vector<ServerInfo> vec, VMInfo vmInfo) {
	//std::cout << vec.size() << std::endl;
	int cpu_max, cpu_min, mem_max, mem_min, cpu_vm, mem_vm;
	vec[0].getCpuNum(cpu_min);
	vec[0].getMemorySize(mem_min);
	vec[vec.size() - 1].getCpuNum(cpu_max);
	vec[vec.size() - 1].getMemorySize(mem_max);
	vmInfo.getCpuNum(cpu_vm);
	vmInfo.getMemorySize(mem_vm);
	double ratio_max = cpu_max * 1.0 / mem_max;
	double ratio_min = cpu_min * 1.0 / mem_min;
	double ratio_vm = cpu_vm * 1.0 / mem_vm;
	if (ratio_vm >= ratio_max) {
		for (int i = vec.size() - 1; i >= 0; i--) {
			ServerObj obj(vec[i], 0);
			if (obj.canDeployOnDoubleNode(vmInfo) || obj.canDeployOnSingleNode(0, vmInfo) || obj.canDeployOnSingleNode(1, vmInfo))
				return vec[i];
		}
	}
	else if (ratio_vm <= ratio_min) {
		for (int i = 0; i < vec.size(); i++) {
			ServerObj obj(vec[i], 0);
			if (obj.canDeployOnDoubleNode(vmInfo) || obj.canDeployOnSingleNode(0, vmInfo) || obj.canDeployOnSingleNode(1, vmInfo))
				return vec[i];
		}
	}
	else {
		int low = 0, high = vec.size() - 1;
		while (low < high) {
			int mid = (low + high) / 2;
			int cpu_mid, mem_mid;
			double ratio_mid;
			vec[mid].getCpuNum(cpu_mid);
			vec[mid].getMemorySize(mem_mid);
			ratio_mid = cpu_mid * 1.0 / mem_mid;
			if (std::abs(ratio_mid - ratio_vm) < 0.01) {
				for (int i = mid; i < vec.size(); i++) {
					ServerObj obj(vec[i], 0);
					if (obj.canDeployOnDoubleNode(vmInfo) || obj.canDeployOnSingleNode(0, vmInfo) || obj.canDeployOnSingleNode(1, vmInfo))
						return vec[i];
				}
			}
			else if (ratio_mid > ratio_vm) {
				high = mid;
			}
			else if (ratio_mid < ratio_vm) {
				low = mid + 1;
			}
		}
		for (int i = low; i < vec.size(); i++) {
			ServerObj obj(vec[i], 0);
			if (obj.canDeployOnDoubleNode(vmInfo) || obj.canDeployOnSingleNode(0, vmInfo) || obj.canDeployOnSingleNode(1, vmInfo))
				return vec[i];
		}
	}
	//避免意外情况

	for (int i = 0; i < vec.size(); i++) {
		ServerObj obj(vec[i], 0);
		if (obj.canDeployOnDoubleNode(vmInfo) || obj.canDeployOnSingleNode(0, vmInfo) || obj.canDeployOnSingleNode(1, vmInfo))
			return vec[i];
	}

	for (int j = 0; j < globalCloud->serverInfoList.size(); j++) {
		ServerObj obj(globalCloud->serverInfoList[j], 0);
		if (obj.canDeployOnSingleNode(0, vmInfo) || obj.canDeployOnSingleNode(1, vmInfo) || obj.canDeployOnDoubleNode(vmInfo))
			return globalCloud->serverInfoList[j];
	}
	//保底
	return vec[0];
}

int Ant_colony::dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) {
	int day = requestsBunch.dayNum;
	std::mt19937 gen(1829);
	std::uniform_real_distribution<> uniform(0.0, 1.0);
	//std::cout << day << std::endl;
	std::vector<OneDayRequest> days_bunch = requestsBunch.bunch;
	for (int i = 0; i < day; i++) {
		std::cerr << "day " << i << std::endl;
		OneDayResult oneDayRes;
		std::vector<Request> addReq;
		std::vector<Request> delReq;
		int addCount = 0;
		for (int j = 0; j < days_bunch[i].size(); j++) {
			auto request = days_bunch[i][j];
			if (request.op == ADD) {
				days_bunch[i][j].assigned = 0;
				days_bunch[i][j].index = addCount;
				addReq.push_back(days_bunch[i][j]);
				addCount++;
			}
			else {
				delReq.push_back(days_bunch[i][j]);
			}
		}
		std::cerr << "addCount "<<addCount << std::endl;
		//handle add requests
		auto serverInfoList = globalCloud->serverInfoList;
		auto vMachineInfoMap = globalCloud->vmInfoMap;
		auto serverObjList = globalCloud->serverObjList;
		int n_ant;
		if (serverObjList.size() == 0) n_ant = NA;
		else n_ant=std::min(NA, int(serverObjList.size()));
		float trail_0;
		//信息素二维矩阵、需求二维矩阵、可能性二维矩阵：服务器 × 虚拟机，可新增服务器行
		std::vector<std::vector<double>> trailMatrix(serverObjList.size(), std::vector<double>(addCount));//tao
		std::vector<std::vector<double>> desireMatrix(serverObjList.size(), std::vector<double>(addCount));//n
		std::vector<std::vector<double>> probMatrix(serverObjList.size(), std::vector<double>(addCount));//p
		std::vector<bool> canBeHold(addCount);
		if (serverObjList.size() == 0)
			trail_0 = 0.5;
		else
			trail_0 = 1.0 / serverObjList.size();//信息素初始值
		//初始化
		for (int p = 0; p < serverObjList.size(); p++) {
			for (int q = 0; q < addCount; q++) {
				trailMatrix[p][q] = trail_0;
				desireMatrix[p][q] = 0;
				probMatrix[p][q] = 0;
			}
		}

		for (int p = 0; p < addCount; p++) {
			canBeHold[p] = 0;
		}
		//std::cout << "fuck you" << std::endl;
		
		std::vector<Request> tmp;
		Local_assign best_assign(-1,1);
		for (int iter = 0; iter < MAX_ITER; iter++) {	
			bool assign_change = 0;
			for (int l = 0; l < n_ant; l++) {
				tmp = addReq;
				auto probableMatrix = probMatrix;
				serverObjList = globalCloud->serverObjList;
				int size = serverObjList.size();
				int res_vm = addCount;//剩余未被分配的虚拟机数量
				//std::cout << res_vm << std::endl;
				std::vector<Deploy_pair> deploy_pairs;
				std::vector<ServerInfo> purchase_servers;//购买的型号
				std::vector<ServerObj> purchase_objs;//当天新购的服务器用于局部更新的实例
				for (int j = l; j < size + l; j++) {
					if (res_vm == 0) break;
					//auto server = serverObjList[j%size];
					int count = j - l;
					while (true) {
						auto beHold = canBeHold;
						if (res_vm <= 0) break;
						bool have_try = 0;//在这一轮次中是否有虚拟机可以fit该服务器
						for (int pos = 0; pos < addCount; pos++) {
							if (probableMatrix[j%size][pos] != -1)
								probableMatrix[j%size][pos] = 0;
						}
						/*
						std::cout << "Probablity_Begin: ";
						for (int p = 0; p < addCount; p++) {
							std::cout << probableMatrix[j%size][p] << " ";
						}
						std::cout << std::endl;
						*/
						for(int pos=0; pos<addCount;pos++) {
							Request req = tmp[pos];
							if (req.assigned==1) continue;//current server have tried all VM
							auto model = req.vMachineModel;
							auto InfoIt = vMachineInfoMap.find(model);
							if (InfoIt == vMachineInfoMap.end()) {
								LOGE("machine model does not exist");
								exit(-1);
							}
							VMInfo machineInfo = InfoIt->second;

							int vmCPU;
							int vmMem;
							machineInfo.getCpuNum(vmCPU);
							machineInfo.getMemorySize(vmMem);
							
							//可以被A节点容纳
							if (serverObjList[j%size].canDeployOnSingleNode(0, machineInfo)) {
								//std::cout << "Yes_A" << std::endl;
								beHold[pos] = 1;
								have_try = 1;
								tmp[pos].assignType = 0;
								//calculate desirability
								double totalWaste = 0.0;
								Resource A_resource, B_resource;
								int totalMemory, totalCpu;
								for (int p = l; p <= j; p++) {
									serverObjList[p%size].getNodeRemainingResource(0, A_resource);
									serverObjList[p%size].getNodeRemainingResource(1, B_resource);
									serverObjList[p%size].info.getCpuNum(totalCpu);
									serverObjList[p%size].info.getMemorySize(totalMemory);
									double fenzi, fenmu;
									if (p < j) {
										//分子
										fenzi = std::abs(2.0*A_resource.cpuNum / totalCpu - 2.0*A_resource.memorySize / totalMemory) +
											std::abs(2.0*B_resource.cpuNum / totalCpu - 2.0*B_resource.memorySize / totalMemory) + 2.0*Epsilon;
										//分母
										fenmu = (totalCpu - 2.0*A_resource.cpuNum) / totalCpu + (totalMemory - 2.0*A_resource.memorySize) / totalMemory
											+ (totalCpu - 2.0*B_resource.cpuNum) / totalCpu + (totalMemory - 2.0*B_resource.memorySize) / totalMemory;
									}
									else {
										fenzi = std::abs(2.0*(A_resource.cpuNum - vmCPU) / totalCpu - 2.0*(A_resource.memorySize - vmMem) / totalMemory) +
											std::abs(2.0*B_resource.cpuNum / totalCpu - 2.0*B_resource.memorySize / totalMemory) + 2.0*Epsilon;
										fenmu = (totalCpu - 2.0*(A_resource.cpuNum - vmCPU)) / totalCpu + (totalMemory - 2.0*(A_resource.memorySize - vmMem)) / totalMemory
											+ (totalCpu - 2.0*B_resource.cpuNum) / totalCpu + (totalMemory - 2.0*B_resource.memorySize) / totalMemory;
									}
									totalWaste += fenzi / fenmu;
								}
								double desire = 1.0 / (Epsilon + totalWaste);
								desireMatrix[j%size][req.index] = desire;

								//calculate probability


							}
							//可以被B节点容纳
							else if (serverObjList[j%size].canDeployOnSingleNode(1, machineInfo)) {
								//std::cout << "Yes_B" << std::endl;
								beHold[pos] = 1;
								have_try = 1;
								tmp[pos].assignType = 1;
								double totalWaste = 0.0;
								Resource A_resource, B_resource;
								int totalMemory, totalCpu;
								for (int p = l; p <= j; p++) {
									serverObjList[p%size].getNodeRemainingResource(0, A_resource);
									serverObjList[p%size].getNodeRemainingResource(1, B_resource);
									serverObjList[p%size].info.getCpuNum(totalCpu);
									serverObjList[p%size].info.getMemorySize(totalMemory);
									double fenzi, fenmu;
									if (p < j) {
										//分子
										fenzi = std::abs(2.0*A_resource.cpuNum / totalCpu - 2.0*A_resource.memorySize / totalMemory) +
											std::abs(2.0*B_resource.cpuNum / totalCpu - 2.0*B_resource.memorySize / totalMemory) + 2.0*Epsilon;
										//分母
										fenmu = (totalCpu - 2.0*A_resource.cpuNum) / totalCpu + (totalMemory - 2.0*A_resource.memorySize) / totalMemory
											+ (totalCpu - 2.0*B_resource.cpuNum) / totalCpu + (totalMemory - 2.0*B_resource.memorySize) / totalMemory;
									}
									else {
										fenzi = std::abs(2.0*(A_resource.cpuNum) / totalCpu - 2.0*(A_resource.memorySize) / totalMemory) +
											std::abs(2.0*(B_resource.cpuNum-vmCPU) / totalCpu - 2.0*(B_resource.memorySize-vmMem) / totalMemory) + 2.0*Epsilon;
										fenmu = (totalCpu - 2.0*(A_resource.cpuNum)) / totalCpu + (totalMemory - 2.0*(A_resource.memorySize)) / totalMemory
											+ (totalCpu - 2.0*(B_resource.cpuNum-vmCPU)) / totalCpu + (totalMemory - 2.0*(B_resource.memorySize-vmMem)) / totalMemory;
									}
									totalWaste += fenzi / fenmu;
								}
								double desire = 1.0 / (Epsilon + totalWaste);
								desireMatrix[j%size][req.index] = desire;
							}
							//可以被双节点容纳
							else if (serverObjList[j%size].canDeployOnDoubleNode(machineInfo)) {
								//std::cout << "Yes_Double" << std::endl;
								beHold[pos] = 1;
								have_try = 1;
								tmp[pos].assignType = 2;
								double totalWaste = 0.0;
								Resource A_resource, B_resource;
								int totalMemory, totalCpu;
								for (int p = l; p <= j; p++) {
									serverObjList[p%size].getNodeRemainingResource(0, A_resource);
									serverObjList[p%size].getNodeRemainingResource(1, B_resource);
									serverObjList[p%size].info.getCpuNum(totalCpu);
									serverObjList[p%size].info.getMemorySize(totalMemory);
									double fenzi, fenmu;
									if (p < j) {
										//分子
										fenzi = std::abs(2.0*A_resource.cpuNum / totalCpu - 2.0*A_resource.memorySize / totalMemory) +
											std::abs(2.0*B_resource.cpuNum / totalCpu - 2.0*B_resource.memorySize / totalMemory) + 2.0*Epsilon;
										//分母
										fenmu = (totalCpu - 2.0*A_resource.cpuNum) / totalCpu + (totalMemory - 2.0*A_resource.memorySize) / totalMemory
											+ (totalCpu - 2.0*B_resource.cpuNum) / totalCpu + (totalMemory - 2.0*B_resource.memorySize) / totalMemory;
									}
									else {
										fenzi = std::abs(2.0*(A_resource.cpuNum - vmCPU/2) / totalCpu - 2.0*(A_resource.memorySize - vmMem/2) / totalMemory) +
											std::abs(2.0*(B_resource.cpuNum - vmCPU / 2) / totalCpu - 2.0*(B_resource.memorySize - vmMem / 2) / totalMemory) + 2.0*Epsilon;
										fenmu = (totalCpu - 2.0*(A_resource.cpuNum - vmCPU/2)) / totalCpu + (totalMemory - 2.0*(A_resource.memorySize - vmMem/2)) / totalMemory
											+ (totalCpu - 2.0*(B_resource.cpuNum-vmCPU/2)) / totalCpu + (totalMemory - 2.0*(B_resource.memorySize-vmMem/2)) / totalMemory;
									}
									//if (fenzi < 0) std::cout << "warning" << std::endl;
									totalWaste += fenzi / fenmu;
								}
								double desire = 1.0 / (Epsilon + totalWaste);
								//if (desire < 0) std::cout << "warning" << std::endl;
								desireMatrix[j%size][req.index] = desire;
							}
						}
						if (!have_try) break;
						float Q = uniform(gen);
						double maxValue = 0;
						double maxIndex = 0;
						double probableTotal = 0;
						for (int p = 0; p < addCount; p++) {
							double tmp_value = Alpha * trailMatrix[j%size][p] + (1 - Alpha)*desireMatrix[j%size][p];
							if (probableMatrix[j%size][p] != -1&&beHold[p]==1) {
								probableTotal += tmp_value;
								probableMatrix[j%size][p] = tmp_value;
							}
						}
						/*
						std::cout << "Probablity: ";
						for (int p = 0; p < addCount; p++) {
							std::cout << probableMatrix[j%size][p] << " ";
						}
						std::cout << std::endl;
						*/
						int cur_assign=0;
						//std::cout << Q << std::endl;
						if (Q <= Q_0) {
							for (int p = 0; p < addCount; p++) {
								double tmp_value = probableMatrix[j%size][p];
								if (tmp_value > maxValue) {
									maxValue = tmp_value;
									maxIndex = p;
								}
							}
							//assign(server : j%size   vm : p)(不更新全局)
							Deploy_pair deploy_pair(j%size, maxIndex,tmp[maxIndex].assignType);
							deploy_pairs.push_back(deploy_pair);
							tmp[maxIndex].assigned = 1;
							probableMatrix[j%size][maxIndex] = -1;
							cur_assign = maxIndex;
							res_vm--;
							//服务器副本更新
							int allocCpu, allocMem;
							auto model = addReq[maxIndex].vMachineModel;
							auto InfoIt = vMachineInfoMap.find(model);
							if (InfoIt == vMachineInfoMap.end()) {
								LOGE("machine model does not exist");
								exit(-1);
							}
							VMInfo machineInfo = InfoIt->second;

							machineInfo.getCpuNum(allocCpu);
							machineInfo.getMemorySize(allocMem);
							//std::cout << maxIndex << " fuck you begin" << std::endl;
							if (tmp[maxIndex].assignType == 0) {
								serverObjList[j%size].nodes[0].remainingResource.allocResource(Resource(allocCpu, allocMem));
							}
							else if (tmp[maxIndex].assignType == 1) {
								serverObjList[j%size].nodes[1].remainingResource.allocResource(Resource(allocCpu, allocMem));
							}
							else if (tmp[maxIndex].assignType == 2) {
								serverObjList[j%size].nodes[0].remainingResource.allocResource(Resource(allocCpu/2, allocMem/2));
								serverObjList[j%size].nodes[1].remainingResource.allocResource(Resource(allocCpu / 2, allocMem / 2));
							}
							//std::cout << maxIndex << " fuck you end" << std::endl;
						}
						else {
							std::vector<double> rotate(addCount + 1);
							rotate[0] = 0;
							rotate[addCount] = 1;
							for (int p = 1; p < addCount; p++) {
								if (probableMatrix[j%size][p - 1] == -1) rotate[p] = rotate[p - 1];
								else
									rotate[p] = rotate[p - 1] + probableMatrix[j%size][p - 1] / probableTotal;
							}
							float P = uniform(gen);
							for (int p = 1; p <= addCount; p++) {
								if (P < rotate[p] && P >= rotate[p - 1]) {
									//assign(server: j%size vm:p-1)(不更新全局)
									Deploy_pair deploy_pair(j%size, p - 1,tmp[p-1].assignType);
									deploy_pairs.push_back(deploy_pair);
									tmp[p-1].assigned = 1;
									probableMatrix[j%size][p - 1] = -1;
									cur_assign = p - 1;
									res_vm--;
									//服务器副本更新
									int allocCpu, allocMem;
									auto model = addReq[p-1].vMachineModel;
									auto InfoIt = vMachineInfoMap.find(model);
									if (InfoIt == vMachineInfoMap.end()) {
										LOGE("machine model does not exist");
										exit(-1);
									}
									VMInfo machineInfo = InfoIt->second;
									machineInfo.getCpuNum(allocCpu);
									machineInfo.getMemorySize(allocMem);
									//std::cout << "holy begin" << std::endl;
									if (tmp[p-1].assignType == 0) {
										serverObjList[j%size].nodes[0].remainingResource.allocResource(Resource(allocCpu, allocMem));
									}
									else if (tmp[p-1].assignType == 1) {
										serverObjList[j%size].nodes[1].remainingResource.allocResource(Resource(allocCpu, allocMem));
									}
									else if (tmp[p-1].assignType == 2) {
										serverObjList[j%size].nodes[0].remainingResource.allocResource(Resource(allocCpu / 2, allocMem / 2));
										serverObjList[j%size].nodes[1].remainingResource.allocResource(Resource(allocCpu / 2, allocMem / 2));
									}
									//std::cout << "holy end" << std::endl;
									break;
								}
							}
						}

						//std::cout << j % size <<" you "<<size<<" me "<<addCount<< std::endl;
						trailMatrix[j%size][cur_assign] = trailMatrix[j%size][cur_assign] * (1 - P_l) + trail_0;
					}
					
				}
				//std::cout << "holyShit:" << n_ant<<" "<<l<<std::endl;
				//当前服务器无法容纳所有虚拟机
				if (res_vm > 0) {
					//std::cout << res_vm << std::endl;
					//std::cout << addCount << std::endl;
					for (int pos = 0; pos < addCount; pos++) {
						auto req = tmp[pos];
						if (req.assigned == 0) {
							auto model = req.vMachineModel;
							auto InfoIt = vMachineInfoMap.find(model);
							if (InfoIt == vMachineInfoMap.end()) {
								LOGE("machine model does not exist");
								exit(-1);
							}
							VMInfo machineInfo = InfoIt->second;
							int vmCpu, vmMem;
							machineInfo.getCpuNum(vmCpu);
							machineInfo.getMemorySize(vmMem);
							bool flag = 0;
							for (int qos = 0; qos < purchase_objs.size(); qos++) {
								//std::cout << "bu hao" << std::endl;
								if (purchase_objs[qos].canDeployOnSingleNode(0, machineInfo)) {
									flag = 1;
									purchase_objs[qos].nodes[0].remainingResource.allocResource(Resource(vmCpu, vmMem));
									Deploy_pair deploy_pair(qos+size, pos,0);
									deploy_pairs.push_back(deploy_pair);
									tmp[pos].assigned = 1;
									res_vm--;
									break;
								}
								else if (purchase_objs[qos].canDeployOnSingleNode(1, machineInfo)) {
									flag = 1;
									purchase_objs[qos].nodes[1].remainingResource.allocResource(Resource(vmCpu, vmMem));
									Deploy_pair deploy_pair(qos + size, pos,1);
									deploy_pairs.push_back(deploy_pair);
									tmp[pos].assigned = 1;
									res_vm--;
									break;
								}
								else if (purchase_objs[qos].canDeployOnDoubleNode(machineInfo)) {
									flag = 1;
									purchase_objs[qos].nodes[0].remainingResource.allocResource(Resource(vmCpu/2, vmMem/2));
									purchase_objs[qos].nodes[1].remainingResource.allocResource(Resource(vmCpu/2, vmMem/2));
									Deploy_pair deploy_pair(qos + size, pos,2);
									deploy_pairs.push_back(deploy_pair);
									tmp[pos].assigned = 1;
									res_vm--;
									break;
								}
							}
							if (!flag) {
								//std::cout << "purchase:happy" << std::endl;
								ServerInfo newServer = binarySearch(globalCloud->serverSortList, machineInfo);
								purchase_servers.push_back(newServer);
								ServerObj newObj(newServer, 0);
								purchase_objs.push_back(newObj);
								if (purchase_objs[purchase_objs.size()-1].canDeployOnSingleNode(0, machineInfo)) {
									purchase_objs[purchase_objs.size() - 1].nodes[0].remainingResource.allocResource(Resource(vmCpu, vmMem));
									Deploy_pair deploy_pair(purchase_objs.size() - 1 + size, pos,0);
									deploy_pairs.push_back(deploy_pair);
									tmp[pos].assigned = 1;
									res_vm--;
								}
								else if (purchase_objs[purchase_objs.size() - 1].canDeployOnSingleNode(1, machineInfo)) {
									purchase_objs[purchase_objs.size() - 1].nodes[1].remainingResource.allocResource(Resource(vmCpu, vmMem));
									Deploy_pair deploy_pair(purchase_objs.size() - 1 + size, pos,1);
									deploy_pairs.push_back(deploy_pair);
									tmp[pos].assigned = 1;
									res_vm--;
								}
								else if (purchase_objs[purchase_objs.size() - 1].canDeployOnDoubleNode(machineInfo)) {
									purchase_objs[purchase_objs.size() - 1].nodes[0].remainingResource.allocResource(Resource(vmCpu / 2, vmMem / 2));
									purchase_objs[purchase_objs.size() - 1].nodes[1].remainingResource.allocResource(Resource(vmCpu / 2, vmMem / 2));
									Deploy_pair deploy_pair(purchase_objs.size() - 1 + size, pos,2);
									deploy_pairs.push_back(deploy_pair);
									tmp[pos].assigned = 1;
									res_vm--;
								}
							}
							
						}
					}
				}

				//更新最佳部署(取watse较小的部署)
				double totalWaste=0.0;
				Resource A_resource, B_resource;
				int totalMemory, totalCpu;
				for (int pos = 0; pos < size; pos++) {
					serverObjList[pos].info.getCpuNum(totalCpu);
					serverObjList[pos].info.getMemorySize(totalMemory);
					serverObjList[pos].getNodeRemainingResource(0, A_resource);
					serverObjList[pos].getNodeRemainingResource(1, B_resource);

					//caculate waste
					//分子
					double fenzi = std::abs(2.0*A_resource.cpuNum / totalCpu - 2.0*A_resource.memorySize / totalMemory) +
						std::abs(2.0*B_resource.cpuNum / totalCpu - 2.0*B_resource.memorySize / totalMemory) + 2.0*Epsilon;
					//分母
					double fenmu = (totalCpu - 2.0*A_resource.cpuNum) / totalCpu + (totalMemory - 2.0*A_resource.memorySize) / totalMemory
						+ (totalCpu - 2.0*B_resource.cpuNum) / totalCpu + (totalMemory - 2.0*B_resource.memorySize) / totalMemory + 2.0*Epsilon;

					totalWaste += fenzi / fenmu;
				}
				std::cerr << " deploy size " << deploy_pairs.size() << std::endl;
				Local_assign local_assign(purchase_servers, deploy_pairs, totalWaste,1);
				if (totalWaste < best_assign.waste || best_assign.waste < 0) {
					best_assign = local_assign;
				}
				else assign_change = 1;
			}
			if (!assign_change) best_assign.iters += 1;
			auto deployBest = best_assign.deploy_pairs;
			std::cerr << " Best_deploy size "<<deployBest.size() << std::endl;
			for (int pos = 0; pos < deployBest.size(); pos++) {
				if (deployBest[pos].index_server < serverObjList.size()) {
					double lambda = MAX_ITER * 1.0 / (iter - best_assign.iters + 1);
					trailMatrix[deployBest[pos].index_server][deployBest[pos].index_vm] =
						(1 - P_g)*trailMatrix[deployBest[pos].index_server][deployBest[pos].index_vm] +
						P_g * lambda / best_assign.waste;
				}
			}
		}

		//全局更新
		//OneDayResult oneDayRes;

		serverObjList = globalCloud->serverObjList;
		std::map<std::string, std::vector<int>> localToGlobal;
		std::map<int, int> indexToID;
		std::vector<int> initValue;
		
		for (int pos = 0; pos < best_assign.serverInfoList.size(); pos++) {
			std::string model;
			best_assign.serverInfoList[pos].getModel(model);
			if (localToGlobal.find(model) == localToGlobal.end()) {
				localToGlobal[model] = initValue;
			}
			localToGlobal[model].push_back(pos + serverObjList.size());
		}

		std::map<std::string, std::vector<int>>::iterator it;
		int curId = serverObjList.size();
		for (int pos = 0; pos < serverObjList.size(); pos++) {
			indexToID[pos] = pos;
		}
		for (it = localToGlobal.begin(); it != localToGlobal.end(); it++) {
			oneDayRes.purchaseMap[it->first] = it->second.size();
			for (int qos = 0; qos < it->second.size(); qos++) {
				indexToID[it->second[qos]] = curId;
				globalCloud->addServerObj(best_assign.serverInfoList[it->second[qos] - serverObjList.size()]);
				curId++;
			}
		}

		serverObjList = globalCloud->serverObjList;
		auto sortDeploy = best_assign.deploy_pairs;
		sort(sortDeploy.begin(), sortDeploy.end());

		//std::cout << "motherfucker" << std::endl;
		for (int pos = 0; pos < sortDeploy.size(); pos++) {
			auto deploy_pair = sortDeploy[pos];
			
			/*
			std::cout << deploy_pair.index_server << " " << deploy_pair.deploy_type << " " << addReq[deploy_pair.index_vm].vMachineModel << " " << addReq[deploy_pair.index_vm].vMachineID << std::endl;
			std::cout << indexToID[deploy_pair.index_server] << std::endl;
			Resource res1, res2;
			serverObjList[indexToID[deploy_pair.index_server]].getNodeRemainingResource(0, res1);
			serverObjList[indexToID[deploy_pair.index_server]].getNodeRemainingResource(1, res2);
			std::cout << res1.cpuNum << " " << res1.memorySize << " " << res2.cpuNum << " " << res2.memorySize << std::endl;
			int allocCpu, allocMem;
			auto InfoIt = vMachineInfoMap.find(addReq[deploy_pair.index_vm].vMachineModel);
			if (InfoIt == vMachineInfoMap.end()) {
				LOGE("machine model does not exist");
				exit(-1);
			}
			VMInfo machineInfo = InfoIt->second;
			machineInfo.getCpuNum(allocCpu);
			machineInfo.getMemorySize(allocMem);
			std::cout << allocCpu << " " << allocMem << std::endl;
			*/

			//std::cout << "begin" << std::endl;
			globalCloud->addVMObj(indexToID[deploy_pair.index_server], deploy_pair.deploy_type, 
				addReq[deploy_pair.index_vm].vMachineModel, addReq[deploy_pair.index_vm].vMachineID);
			//std::cout << "end" << std::endl;
			Deploy res;
			
			res.node = deploy_pair.deploy_type==2 ? -1:deploy_pair.deploy_type;
			res.serverID = indexToID[deploy_pair.index_server];
			oneDayRes.deployList.push_back(res);
		}

		//Handle Delete
		for (int pos = 0; pos < delReq.size(); pos++) {
			globalCloud->delVMObj(delReq[pos].vMachineID);
		}

		receiver.push_back(oneDayRes);
	}
	return 0;
}



bool operator <(const Deploy_pair& pair1, const Deploy_pair& pair2) {
	return pair1.index_vm < pair2.index_vm;
}



