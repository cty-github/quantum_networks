//
// Created by TianyaoChu on 2023/3/29.
//

#include "qedge.h"
#include <cmath>

QEdge::QEdge(int node_id_a, int node_id_b, int capacity, double distance, double decay_rate):
        node_id_a(node_id_a), node_id_b(node_id_b), capacity(capacity), channel_occupied(0),
        distance(distance), decay_rate(decay_rate), success_probability(exp(-decay_rate*distance)) {}

QEdge::QEdge(const QEdge& edge) {
    node_id_a = edge.node_id_a;
    node_id_b = edge.node_id_b;
    capacity = edge.capacity;
    channel_occupied = edge.channel_occupied;
    distance = edge.distance;
    decay_rate = edge.decay_rate;
    success_probability = edge.success_probability;
}

QEdge::~QEdge() = default;

bool QEdge::connect_node(int node_id) const {
    return node_id_a == node_id || node_id_b == node_id;
}

int QEdge::get_node_id_a() const {
    return node_id_a;
}

int QEdge::get_node_id_b() const {
    return node_id_b;
}

int QEdge::get_capacity() const {
    return capacity;
}

bool QEdge::occupy_channel(int n_qubit) {
    if (capacity - channel_occupied >= n_qubit) {
        channel_occupied += n_qubit;
        return true;
    } else {
        return false;
    }
}

double QEdge::get_distance() const {
    return distance;
}

double QEdge::get_decay_rate() const {
    return decay_rate;
}

double QEdge::get_success_probability() const {
    return success_probability;
}
