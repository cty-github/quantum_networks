//
// Created by TianyaoChu on 2023/5/7.
//

#include "net_resource.h"
#include <iostream>

NetResource::NetResource(NetTopology* net_topo): net_topo(net_topo) {
    for (int i = 0; i < net_topo->get_node_num(); i++) {
        free_node_memory[i] = net_topo->get_node(i)->get_memory_size();
        for (auto j:net_topo->get_node(i)->get_adjacent_nodes_id()) {
            if (i < j) {
                QEdge* edge = net_topo->get_edge(i, j);
                free_edge_capacity[edge->get_edge_id()] = edge->get_capacity();
            }
        }
    }
}

NetResource::~NetResource() = default;

int NetResource::get_free_node_memory(int node_id) const {
    if (free_node_memory.find(node_id) == free_node_memory.end()) {
        cout << "No Node " << node_id << endl;
        return -1;
    }
    return free_node_memory.find(node_id)->second;
}

int NetResource::get_free_edge_capacity(int edge_id) const {
    if (free_edge_capacity.find(edge_id) == free_edge_capacity.end()) {
        cout << "No Edge " << edge_id << endl;
        return -1;
    }
    return free_edge_capacity.find(edge_id)->second;
}

bool NetResource::reserve_node_memory(int node_id, int num) {
    if (get_free_node_memory(node_id) < num) {
        cout << "No Enough Memory in Node " << node_id << endl;
        return false;
    }
    free_node_memory[node_id] -= num;
    return true;
}

bool NetResource::reserve_edge_capacity(int edge_id, int num) {
    if (get_free_edge_capacity(edge_id) < num) {
        cout << "No Enough Capacity in Edge " << edge_id << endl;
        return false;
    }
    free_edge_capacity[edge_id] -= num;
    return true;
}

bool NetResource::release_node_memory(int node_id, int num) {
    if (get_free_node_memory(node_id)+num > net_topo->get_node(node_id)->get_memory_size()) {
        cout << "Exceed Memory of Node " << node_id << endl;
        return false;
    }
    free_node_memory[node_id] += num;
    return true;
}

bool NetResource::release_edge_capacity(int edge_id, int num) {
    if (get_free_edge_capacity(edge_id)+num > net_topo->get_edge(edge_id)->get_capacity()) {
        cout << "Exceed Capacity of Edge " << edge_id << endl;
        return false;
    }
    free_edge_capacity[edge_id] += num;
    return true;
}

bool NetResource::check_link_resource(int s_id, int d_id, int num) const {
    if (get_free_node_memory(s_id) < num) {
        cout << "No Enough Memory in Node " << s_id << endl;
        return false;
    }
    if (get_free_node_memory(d_id) < num) {
        cout << "No Enough Memory in Node " << d_id << endl;
        return false;
    }
    int edge_id = net_topo->get_edge(s_id, d_id)->get_edge_id();
    if (get_free_edge_capacity(edge_id) < num) {
        cout << "No Enough Capacity in Edge " << edge_id << endl;
        return false;
    }
    return true;
}

int NetResource::max_link_resource(int s_id, int d_id) const {
    int edge_id = net_topo->get_edge(s_id, d_id)->get_edge_id();
    int max_num = get_free_edge_capacity(edge_id);
    if (net_topo->get_node(s_id)->is_user()) {
        if (max_num > get_free_node_memory(s_id)) {
            max_num = get_free_node_memory(s_id);
        }
    } else {
        if (max_num > get_free_node_memory(s_id)/2) {
            max_num = get_free_node_memory(s_id)/2;
        }
    }
    if (net_topo->get_node(d_id)->is_user()) {
        if (max_num > get_free_node_memory(d_id)) {
            max_num = get_free_node_memory(d_id);
        }
    } else {
        if (max_num > get_free_node_memory(d_id)/2) {
            max_num = get_free_node_memory(d_id)/2;
        }
    }
    return max_num;
}

bool NetResource::reserve_link_resource(int s_id, int d_id, int num) {
    if (!reserve_node_memory(s_id, num)) {
        return false;
    }
    if (!reserve_node_memory(d_id, num)) {
        release_node_memory(s_id, num);
        return false;
    }
    int edge_id = net_topo->get_edge(s_id, d_id)->get_edge_id();
    if (!reserve_edge_capacity(edge_id, num)) {
        release_node_memory(s_id, num);
        release_node_memory(d_id, num);
        return false;
    }
    return true;
}

bool NetResource::release_link_resource(int s_id, int d_id, int num) {
    release_node_memory(s_id, num);
    release_node_memory(s_id, num);
    int edge_id = net_topo->get_edge(s_id, d_id)->get_edge_id();
    release_edge_capacity(edge_id, num);
    return true;
}
