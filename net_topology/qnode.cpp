//
// Created by TianyaoChu on 2023/3/29.
//

#include "qnode.h"

QNode::QNode(int id, NodeType node_type, double pos_x, double pos_y,
             int memory_size, double success_probability):
        node_type(node_type), id(id), pos_x(pos_x), pos_y(pos_y),
        memory_size(memory_size), memory_occupied(0), success_probability(success_probability) {}

QNode::QNode(const QNode& node) {
    node_type = node.node_type;
    id = node.id;
    pos_x = node.pos_x;
    pos_y = node.pos_y;
    memory_size = node.memory_size;
    memory_occupied = node.memory_occupied;
    success_probability = node.success_probability;
    adjacent_nodes_id = node.adjacent_nodes_id;
}

QNode::~QNode() = default;

bool QNode::is_user() const {
    return node_type == User;
}

bool QNode::is_repeater() const {
    return node_type == Repeater;
}

int QNode::get_id() const {
    return id;
}

double QNode::get_pos_x() const {
    return pos_x;
}

double QNode::get_pos_y() const {
    return pos_y;
}

int QNode::get_memory_size() const {
    return memory_size;
}

bool QNode::occupy_memory(int n_qubit) {
    if (memory_size - memory_occupied >= n_qubit) {
        memory_occupied += n_qubit;
        return true;
    } else {
        return false;
    }
}

double QNode::get_success_probability() const {
    return success_probability;
}

vector<int> QNode::get_adjacent_nodes_id() const {
    return adjacent_nodes_id;
}

bool QNode::add_adjacent_node(int node_id) {
    adjacent_nodes_id.push_back(node_id);
    return true;
}
