#include <iostream>
#include <dirent.h>
#include "qnetwork.h"

using namespace std;

int main() {
    string proj_path = "D:/Quantum Technology and Device/Quantum Networks/Code/quantum_networks/";
    string current_time_stamp = get_current_time_stamp();

//    // example network
//    QNetwork qnetwork = QNetwork(proj_path+"data/example/net_dev_example.txt",
//                                 proj_path+"data/example/net_topo_example.txt",
//                                 proj_path+"data/example/sd_pair_example.txt",
//                                 proj_path+"data/example/output/output_"+current_time_stamp+".txt");

    // test network
    string test_path = proj_path+"data/test/"+current_time_stamp+"/";
    mkdir(test_path.c_str());
    QNetwork qnetwork = QNetwork(10, 10, 10, 20,
                                 test_path+"output_"+current_time_stamp+".txt");
    qnetwork.save_net(test_path+"net_dev_"+current_time_stamp+".txt",
                      test_path+"net_topo_"+current_time_stamp+".txt",
                      test_path+"sd_pair_"+current_time_stamp+".txt");

    qnetwork.initialize(3);
    while (qnetwork.work_cycle(5)) {}

    cout << "--------------------------" << endl;
    cout << "Finished Connection Num: " << qnetwork.get_finished_cxn_num() << endl;

    return 0;
}
