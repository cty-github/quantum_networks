//
// Created by TianyaoChu on 2023/3/29.
//

#ifndef QUANTUM_NETWORKS_QNODE_H
#define QUANTUM_NETWORKS_QNODE_H

#include <vector>

using namespace std;

enum NodeType{User, Repeater, Unknown=-1};

class QNode {
private:
    NodeType node_type;
    int id;
    double pos_x;
    double pos_y;
    int memory_size;
    int memory_occupied;
    double success_probability;
    vector<int> adjacent_nodes_id;
public:
    explicit QNode(int id, NodeType node_type=User, double pos_x=0.0, double pos_y=0.0,
                   int memory_size=10, double success_probability=0.9);
    QNode(const QNode& node);
    ~QNode();
    bool is_user() const;
    bool is_repeater() const;
    int get_id() const;
    double get_pos_x() const;
    double get_pos_y() const;
    int get_memory_size() const;
    bool occupy_memory(int n_qubit);
    double get_success_probability() const;
    vector<int> get_adjacent_nodes_id() const;
    bool add_adjacent_node(int node_id);
};

#endif //QUANTUM_NETWORKS_QNODE_H
