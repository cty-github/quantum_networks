#include <iostream>
#include "qnetwork.h"

using namespace std;

int main() {
    string proj_path = "D:/Quantum Technology and Device/Quantum Networks/Code/quantum_networks/";

//    QNetwork qnetwork = QNetwork(10, 10, 10, 20);
//    qnetwork.save_net(proj_path+"data/net_dev_example.txt",
//                      proj_path+"data/net_topo_example.txt",
//                      proj_path+"data/request_example.txt");
    QNetwork qnetwork = QNetwork(proj_path+"data/net_dev_example.txt",
                                 proj_path+"data/net_topo_example.txt",
                                 proj_path+"data/request_example.txt");
    qnetwork.initialize(3);
}
