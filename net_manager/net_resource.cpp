//
// Created by TianyaoChu on 2023/5/7.
//

#include "net_resource.h"

NetResource::NetResource(NetTopology* net_topo): net_topo(net_topo) {
    for (int i = 0; i < net_topo->get_node_num(); i++) {
        node_memory.push_back(net_topo->get_node(i)->get_memory_size());
        vector<int> edge_i_capacity;
        for (auto j:net_topo->get_node(i)->get_adjacent_nodes_id()) {
            edge_i_capacity.push_back(net_topo->get_edge(i, j)->get_capacity());
        }
        edge_capacity.push_back(edge_i_capacity);
    }
}

NetResource::~NetResource() = default;
