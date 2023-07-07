#include <iostream>
#include <sys/stat.h>
#include "qnetwork.h"
#include "test_params.h"

using namespace std;

int ROUTE_STRTG;   // route strategy type
int RSRC_MANAGE;   // net rsrc manager type

int main() {
    ROUTE_STRTG = ROUTE_STRTG_;
    RSRC_MANAGE = RSRC_MANAGE_;

    string proj_path = "/Users/chutianyao/Data/Quantum Technology and Device/Quantum Networks/code/quantum_networks/";
    string current_time_stamp = get_current_time_stamp();

//    // example network
//    QNetwork qnetwork = QNetwork(proj_path+"data/example/net_dev_example.txt",
//                                 proj_path+"data/example/net_topo_example.txt",
//                                 proj_path+"data/example/sd_pair_example.txt",
//                                 proj_path+"data/example/output/runtime_"+current_time_stamp+".txt",
//                                 proj_path+"data/example/output/metric_"+current_time_stamp+".txt");

    // test network
    string test_path = proj_path+"data/test/"+current_time_stamp+"/";
    mkdir(test_path.c_str(), 0777);
    QNetwork qnetwork = QNetwork(PTNSRC_NUM, BSM_NUM,
                                 USER_NUM, REPEATER_NUM,
                                 NET_SIZE, ALPHA, BETA,
                                 test_path+"runtime_"+current_time_stamp+".txt",
                                 test_path+"metric_"+current_time_stamp+".txt");
    qnetwork.save_net(test_path+"net_dev_"+current_time_stamp+".txt",
                      test_path+"net_topo_"+current_time_stamp+".txt",
                      test_path+"sd_pair_"+current_time_stamp+".txt");

    qnetwork.initialize(CANDIDATE_NUM);
    while (qnetwork.work_cycle(RUN_TIME)) {}
    qnetwork.finish();

    return 0;
}
