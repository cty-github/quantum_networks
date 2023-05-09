//
// Created by TianyaoChu on 2023/4/5.
//

#ifndef QUANTUM_NETWORKS_PATH_H
#define QUANTUM_NETWORKS_PATH_H

#include "qnode.h"
#include "qedge.h"
#include <vector>

class Path {
private:
    double cost;
    vector<QNode*> nodes;
    vector<QEdge*> edges;
    bool visit;
public:
    Path(double cost, vector<QNode*>& nodes, vector<QEdge*>& edges);
    Path(const Path& path);
    double get_cost() const;
    void set_cost(double new_cost);
    static double combine_cost(double cost_a, double cost_b);
    vector<QNode*> get_nodes() const;
    void set_nodes(vector<QNode*>& new_nodes);
    vector<QEdge*> get_edges() const;
    void set_edges(vector<QEdge*>& new_edges);
    bool get_visit() const;
    void set_visit();
    QNode* get_next_node(int node_id) const;
    int get_next_node_id(int node_id) const;
    int get_start_node_id() const;
    int get_end_node_id() const;
    void append_node(QNode* new_node, QEdge* new_edge, double new_cost);
    void print_path() const;
    void insert_path(Path* precede_path);
    void append_path(Path* follow_path);

    struct cmp_path{
        bool operator() (pair<double, Path*> r1, pair<double, Path*> r2) {
            return r1.first < r2.first; // high cost first
        }
    };
};

#endif //QUANTUM_NETWORKS_PATH_H
