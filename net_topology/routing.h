//
// Created by TianyaoChu on 2023/4/5.
//

#ifndef QUANTUM_NETWORKS_ROUTING_H
#define QUANTUM_NETWORKS_ROUTING_H

#include "qnode.h"
#include "qedge.h"
#include <vector>

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
    static double combine_cost(double cost_a, double cost_b);
    vector<QNode*> get_path() const;
    void set_path(vector<QNode*>& new_path);
    bool get_visit() const;
    void set_visit();
    QNode* get_next_node(int node_id) const;
    int get_next_node_id(int node_id) const;
    int get_start_node_id() const;
    int get_end_node_id() const;
    void append_node(QNode* new_node, double new_cost);
    void print_routing() const;
    void insert_routing(Routing* precede_route);
    void append_routing(Routing* follow_route);

    struct cmp_routing{
        bool operator() (pair<double, Routing*> r1, pair<double, Routing*> r2) {
            return r1.first < r2.first; // high cost first
        }
    };
};

#endif //QUANTUM_NETWORKS_ROUTING_H
