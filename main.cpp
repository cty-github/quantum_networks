#include <iostream>
#include "qnetwork.h"

using namespace std;

int main() {
    string proj_path = "D:/Quantum Technology and Device/Quantum Networks/Code/quantum_networks/";

//    QNetwork qnetwork_file = QNetwork(proj_path+"data/net_topo_example.txt");
//    cout << "Node num: " << qnetwork_file.get_node_num() << endl;
//    cout << "Edge num: " << qnetwork_file.get_edge_num() << endl;
//    cout << endl;

    QNetwork qnetwork_args = QNetwork(3, 3, 10, 10);
    cout << "Node num: " << qnetwork_args.get_node_num() << endl;
    cout << "Edge num: " << qnetwork_args.get_edge_num() << endl;
//    qnetwork_args.save_net_topo(proj_path+"data/net_topo_args.txt");
    cout << endl;

    for (int i = 0; i < 10; i++) {
        for (int j = i+1; j < 10; j++) {
            cout << "Shortest Route from " << i << " to " << j << endl;
            Routing* route = qnetwork_args.get_routing(i, j);
            route->print_routing();
            EntangleRoute* etg_route = qnetwork_args.generate_etg_route(route);
            etg_route->print_etg_link();
            cout << endl;
        }
    }
}
