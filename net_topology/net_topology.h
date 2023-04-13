//
// Created by TianyaoChu on 2023/3/29.
//

#ifndef QUANTUM_NETWORKS_NET_TOPOLOGY_H
#define QUANTUM_NETWORKS_NET_TOPOLOGY_H

#include "qnode.h"
#include "qedge.h"
#include "routing.h"
#include "qdevice/device_manager.h"
#include <string>
#include <vector>
#include <map>
#include <set>

class NetTopology {
private:
    int node_num;
    int edge_num;
    map<int, QNode*> nodes;
    map<int, map<int, QEdge*>> edges;
public:
    NetTopology();
    NetTopology(DeviceManager* dev_mgr, int user_num, int repeater_num,
                double size=10000, double alpha=0.5, double beta=0.9);
    NetTopology(const string& filepath, DeviceManager* dev_mgr);
    ~NetTopology();
    int get_node_num() const;
    int get_edge_num() const;
    bool add_node(int id, NodeType node_type, double pos_x, double pos_y, int memory_size, BSM* bsm);
    bool add_node(const QNode &node);
    double get_distance(int node_id_a, int node_id_b) const;
    bool add_edge(int node_id_a, int node_id_b, int capacity,
                  double distance, double success_rate, PhotonSource* ptn_src);
    bool add_edge(const QEdge &edge);
    QNode* get_node(int node_id);
    QEdge* get_edge(int node_id_a, int node_id_b);
    bool save_net(const string& filepath) const;
    Routing* get_routing(int src_node_id, int dst_node_id,
                         const set<int>& closed_nodes={},
                         const set<int>& closed_edge_nodes={});
    vector<Routing*> get_routings(int src_node_id, int dst_node_id, int k);
};

#endif //QUANTUM_NETWORKS_NET_TOPOLOGY_H
