#include <map>
#include <string>
#include <queue>
#include "strategy.h"
#include "virtual-machine.h"

class Ant_colony : public Strategy {
	std::map<std::string, int> purchaseMap;

	int dispatch(RequestsBunch &requestsBunch, std::vector<OneDayResult> &receiver) override;

	int HandleAdd(Request &req, OneDayResult &receiver);

	int HandleDel(Request &del, OneDayResult &receiver);
};

struct Deploy_pair{
	int index_server;
	int index_vm;
	int deploy_type;//0 for A, 1 for B, 2 for double
	Deploy_pair(int _index_server, int _index_vm, int _deploy_type) { index_server = _index_server; index_vm = _index_vm; deploy_type = _deploy_type; }
};

struct Local_assign {
	std::vector<ServerInfo> serverInfoList;
	std::vector<Deploy_pair>deploy_pairs;
	double waste;
	int iters;//¡Ù¥Ê ±º‰
	Local_assign(std::vector<ServerInfo> _serverInfoList, std::vector<Deploy_pair> _deploy_pairs, double _waste, double _iters) {
		serverInfoList = _serverInfoList;
		deploy_pairs = _deploy_pairs;
		waste = _waste;
		iters = _iters;
	}

	Local_assign(double _waste, double _iters) {
		waste = _waste;
		iters = _iters;
	}
};