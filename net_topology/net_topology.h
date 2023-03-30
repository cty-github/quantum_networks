//
// Created by TianyaoChu on 2023/3/29.
//

#ifndef QUANTUM_NETWORKS_NET_TOPOLOGY_H
#define QUANTUM_NETWORKS_NET_TOPOLOGY_H

#include "qnode.h"
#include "qedge.h"
#include <string>
#include <vector>
#include <map>

class Routing {
private:
    double cost;
    vector<QNode*> path;
    bool visit;
public:
    Routing(double cost, vector<QNode*>& path);
    Routing(const Routing& routing);
    double get_cost() const;
    void set_cost(double new_cost);
    vector<QNode*> get_path() const;
    void set_path(vector<QNode*>& new_path);
    bool get_visit() const;
    void set_visit();
};

class NetTopology {
private:
    int node_num;
    int edge_num;
    map<int, QNode*> nodes;
    map<int, map<int, QEdge*>> edges;
public:
    NetTopology();
    NetTopology(int user_num, int repeater_num, double size=10000, double alpha=0.5, double beta=0.9);
    explicit NetTopology(const string& filepath);
    ~NetTopology();
    int get_node_num() const;
    int get_edge_num() const;
    bool add_node(int id, NodeType node_type=User, double pos_x=0.0, double pos_y=0.0,
                  int memory_size=10, double success_probability=0.9);
    bool add_node(const QNode &node);
    double get_distance(int node_id_a, int node_id_b) const;
    bool add_edge(int node_id_a, int node_id_b, int capacity=10,
                  double distance=1200, double decay_rate=0.0002);
    bool add_edge(const QEdge &edge);
    QNode* get_node(int node_id);
    QEdge* get_edge(int node_id_a, int node_id_b);
    bool save_net(const string& filepath) const;
    Routing* get_routing(int src_node_id, int dst_node_id);
    vector<Routing*> get_routings(int src_node_id, int dst_node_id, int k);
};

#endif //QUANTUM_NETWORKS_NET_TOPOLOGY_H
