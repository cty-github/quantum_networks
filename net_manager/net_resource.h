//
// Created by TianyaoChu on 2023/5/7.
//

#ifndef QUANTUM_NETWORKS_NET_RESOURCE_H
#define QUANTUM_NETWORKS_NET_RESOURCE_H

#include "net_topology/net_topology.h"
#include <vector>

class NetResource {
private:
    NetTopology* net_topo;
    vector<int> node_memory;
    vector<vector<int>> edge_capacity;
public:
    explicit NetResource(NetTopology* net_topo);
    ~NetResource();
};

#endif //QUANTUM_NETWORKS_NET_RESOURCE_H
