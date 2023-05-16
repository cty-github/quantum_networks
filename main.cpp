#include <iostream>
#include "qnetwork.h"

using namespace std;

int main() {
    string proj_path = "D:/Quantum Technology and Device/Quantum Networks/Code/quantum_networks/";

//    QNetwork qnetwork = QNetwork(10, 10, 10, 20, proj_path+"data/output/output_"+get_current_time_stamp()+".txt");
    QNetwork qnetwork = QNetwork(proj_path+"data/example/net_dev_example.txt",
                                 proj_path+"data/example/net_topo_example.txt",
                                 proj_path+"data/example/sd_pair_example.txt",
                                 proj_path+"data/output/output_"+get_current_time_stamp()+".txt");

//    qnetwork.save_net(proj_path+"data/example/net_dev_example.txt",
//                      proj_path+"data/example/net_topo_example.txt",
//                      proj_path+"data/example/sd_pair_example.txt");

    qnetwork.initialize(3);
    while (qnetwork.work_cycle(10)) {}

    cout << "--------------------------" << endl;
    cout << "Finished Connection Num: " << qnetwork.get_finished_cxn_num() << endl;

    return 0;
}
