#include <iostream>
#include "qnetwork.h"

using namespace std;

int main() {
    string proj_path = "D:/Quantum Technology and Device/Quantum Networks/Code/quantum_networks/";

    QNetwork qnetwork_args = QNetwork(10, 10);
    cout << "Node num: " << qnetwork_args.get_node_num() << endl;
    cout << "Edge num: " << qnetwork_args.get_edge_num() << endl;
    qnetwork_args.save_net_topo(proj_path+"data/net_topo_args.txt");

    QNetwork qnetwork_file = QNetwork(proj_path+"data/net_topo_example.txt");
    cout << "Node num: " << qnetwork_file.get_node_num() << endl;
    cout << "Edge num: " << qnetwork_file.get_edge_num() << endl;
    Routing* routing = qnetwork_file.get_routing(0, 1);
    cout << routing->get_cost();
    for (auto node:routing->get_path()) {
        cout << " " << node->get_id();
    }
    cout << endl;
}
