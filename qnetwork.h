//
// Created by TianyaoChu on 2023/3/25.
//

#ifndef QUANTUM_NETWORKS_QNETWORK_H
#define QUANTUM_NETWORKS_QNETWORK_H

#include "net_topology/net_topology.h"

class QNetwork {
private:
    NetTopology* net_topology{};
public:
    QNetwork();
    QNetwork(int user_num, int repeater_num, double size=10000, double alpha=0.5, double beta=0.9);
    explicit QNetwork(const string& net_topo_filepath);
    ~QNetwork();
    int get_node_num() const;
    int get_edge_num() const;
    bool save_net_topo(const string& filepath) const;
    Routing* get_routing(int src_node_id, int dst_node_id);
    vector<Routing*> get_routings(int src_node_id, int dst_node_id, int k);
};

#endif //QUANTUM_NETWORKS_QNETWORK_H
