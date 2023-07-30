#include <iostream>
#include <dirent.h>
#include <filesystem>
#include <sys/stat.h>
#include "qnetwork.h"
#include "test_params.h"

using namespace std;
using namespace std::__fs::filesystem;

int main() {
    string proj_path = "/Users/chutianyao/Data/Quantum Technology and Device/Quantum Networks/code/quantum_networks/";

//    // prepare network
//    string exp_case = "decay-rate-"+to_string(PTN_DECAY_RATE*1000).substr(0,3);
//    string exp_path = proj_path+"data/experiment/"+exp_case+"/";
//    mkdir(exp_path.c_str(), 0777);
//    QNetwork qnetwork = QNetwork(PTNSRC_NUM, BSM_NUM,
//                                 USER_NUM, REPEATER_NUM,
//                                 NET_SIZE, ALPHA, BETA,
//                                 exp_path+"param_"+exp_case+".txt",
//                                 exp_path+"runtime_"+current_time_stamp+".txt",
//                                 exp_path+"metric_"+current_time_stamp+".txt");
//    qnetwork.save_net(exp_path+"net_dev_"+exp_case+".txt",
//                      exp_path+"net_topo_"+exp_case+".txt",
//                      exp_path+"sd_pair_"+exp_case+".txt");

//    // example network
//    string exp_param = "decay-rate";
//    string exp_case = "decay-rate"+to_string(PTN_DECAY_RATE*1000).substr(0,3);
//    string exp_bench = "work-"+to_string(WORK_CYCLE)+"_route-"+to_string(ROUTE_STRTG);
//    string exp_path = proj_path+"data/experiment/"+exp_case+"/";
//    QNetwork qnetwork = QNetwork(exp_path+"net_dev_"+exp_case+".txt",
//                                 exp_path+"net_topo_"+exp_case+".txt",
//                                 exp_path+"sd_pair_"+exp_case+".txt",
//                                 exp_path+"runtime_"+exp_bench+"_"+current_time_stamp+".txt",
//                                 exp_path+"metric_"+exp_bench+"_"+current_time_stamp+".txt");

    // test network
    string current_time_stamp = get_current_time_stamp();
    string test_path = proj_path+"data/test/"+current_time_stamp+"/";
    mkdir(test_path.c_str(), 0777);
    QNetwork qnetwork = QNetwork(PTNSRC_NUM, BSM_NUM,
                                 USER_NUM, REPEATER_NUM,
                                 NET_SIZE, ALPHA, BETA,
                                 test_path+"param_"+current_time_stamp+".txt",
                                 test_path+"runtime_"+current_time_stamp+".txt",
                                 test_path+"metric_"+current_time_stamp+".txt");
    qnetwork.save_net(test_path+"net_dev_"+current_time_stamp+".txt",
                      test_path+"net_topo_"+current_time_stamp+".txt",
                      test_path+"sd_pair_"+current_time_stamp+".txt");
    qnetwork.initialize(CANDIDATE_NUM);
    switch (WORK_CYCLE) {
        case 0: {
            while (qnetwork.dynamic_cycle()) {}
            break;
        }
        case 1: {
            while (qnetwork.static_cycle()) {}
            break;
        }
        default: {
            throw logic_error("Unknown WORK_CYCLE");
        }
    }
    qnetwork.finish();

//    // run experiments
//    string exp_case = "decay-rate-"+to_string(PTN_DECAY_RATE*1000).substr(0,3);
//    string exp_bench = "work-"+to_string(WORK_CYCLE)+"_route-"+to_string(ROUTE_STRTG);
//    string exp_path = proj_path+"data/experiment/"+exp_case+"/";
//    mkdir(exp_path.c_str(), 0777);
//    for (int i = 0; i < 5; i++) {
//        string current_time_stamp = get_current_time_stamp();
//        QNetwork qnetwork = QNetwork(PTNSRC_NUM, BSM_NUM,
//                                     USER_NUM, REPEATER_NUM,
//                                     NET_SIZE, ALPHA, BETA,
//                                     exp_path+"param_"+exp_bench+"_"+current_time_stamp+".txt",
//                                     exp_path+"runtime_"+exp_bench+"_"+current_time_stamp+".txt",
//                                     exp_path+"metric_"+exp_bench+"_"+current_time_stamp+".txt");
//        qnetwork.save_net(exp_path+"net_dev_"+exp_bench+"_"+current_time_stamp+".txt",
//                          exp_path+"net_topo_"+exp_bench+"_"+current_time_stamp+".txt",
//                          exp_path+"sd_pair_"+exp_bench+"_"+current_time_stamp+".txt");
//        qnetwork.initialize(CANDIDATE_NUM);
//        switch (WORK_CYCLE) {
//            case 0: {
//                while (qnetwork.dynamic_cycle()) {}
//                break;
//            }
//            case 1: {
//                while (qnetwork.static_cycle()) {}
//                break;
//            }
//            default: {
//                throw logic_error("Unknown WORK_CYCLE");
//            }
//        }
//        qnetwork.finish();
//    }

    return 0;
}
