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
    map<int, int> free_node_memory;
    map<int, int> free_edge_capacity;
public:
    explicit NetResource(NetTopology* net_topo);
    ~NetResource();
    int get_free_node_memory(int node_id) const;
    int get_free_edge_capacity(int edge_id) const;
    bool reserve_node_memory(int node_id, int num);
    bool reserve_edge_capacity(int edge_id, int num);
    bool release_node_memory(int node_id, int num);
    bool release_edge_capacity(int edge_id, int num);
    bool check_link_resource(int s_id, int d_id, int num) const;
    int max_link_resource(int s_id, int d_id) const;
    bool reserve_link_resource(int s_id, int d_id, int num);
    bool release_link_resource(int s_id, int d_id, int num);
};

#endif //QUANTUM_NETWORKS_NET_RESOURCE_H
