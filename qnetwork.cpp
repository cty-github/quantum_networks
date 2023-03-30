//
// Created by TianyaoChu on 2023/3/25.
//

#include "qnetwork.h"
#include <iostream>

QNetwork::QNetwork() = default;

QNetwork::QNetwork(int user_num, int repeater_num, double size, double alpha, double beta) {
    cout << "Construct QNetwork with Args" << endl;
    net_topology = new NetTopology(user_num, repeater_num, size, alpha, beta);
}

QNetwork::QNetwork(const string& net_topo_filepath) {
    cout << "Construct QNetwork with File " << net_topo_filepath << endl;
    net_topology = new NetTopology(net_topo_filepath);
}

QNetwork::~QNetwork() = default;

int QNetwork::get_node_num() const {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return 0;
    }
    return net_topology->get_node_num();
}

int QNetwork::get_edge_num() const {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return 0;
    }
    return net_topology->get_edge_num();
}

bool QNetwork::save_net_topo(const string& filepath) const {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return false;
    }
    return net_topology->save_net(filepath);
}

Routing* QNetwork::get_routing(int src_node_id, int dst_node_id) {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    return net_topology->get_routing(src_node_id, dst_node_id);
}

vector<Routing*> QNetwork::get_routings(int src_node_id, int dst_node_id, int k) {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    return net_topology->get_routings(src_node_id, dst_node_id, k);
}
