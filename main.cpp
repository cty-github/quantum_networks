#include <iostream>
#include "qnetwork.h"

using namespace std;

int main() {
    string proj_path = "D:/Quantum Technology and Device/Quantum Networks/Code/quantum_networks/";

//    QNetwork qnetwork = QNetwork(10, 10, 10, 20);
    QNetwork qnetwork = QNetwork(proj_path+"data/net_dev_example.txt",
                                 proj_path+"data/net_topo_example.txt",
                                 proj_path+"data/sd_pair_example.txt");

//    qnetwork.save_net(proj_path+"data/net_dev_example.txt",
//                      proj_path+"data/net_topo_example.txt",
//                      proj_path+"data/sd_pair_example.txt");

    qnetwork.initialize(3);

//    int cycle_num = 10;
//    while(cycle_num > 0) {
//        qnetwork.work_cycle();
//        cycle_num--;
//    }
    while (qnetwork.work_cycle()) {}

    return 0;
}
