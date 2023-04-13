//
// Created by TianyaoChu on 2023/3/29.
//

#include "qedge.h"

QEdge::QEdge(int node_id_a, int node_id_b, int capacity, double distance, double success_rate, PhotonSource* ptn_src):
node_id_a(node_id_a), node_id_b(node_id_b),
capacity(capacity), channel_occupied(0),
distance(distance), decay_rate(ptn_src->get_decay_rate()),
success_rate(success_rate), ptn_src(ptn_src) {}

QEdge::QEdge(const QEdge& edge) {
    node_id_a = edge.node_id_a;
    node_id_b = edge.node_id_b;
    capacity = edge.capacity;
    channel_occupied = edge.channel_occupied;
    distance = edge.distance;
    decay_rate = edge.decay_rate;
    success_rate = edge.success_rate;
    ptn_src = edge.ptn_src;
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

double QEdge::get_success_rate() const {
    return success_rate;
}

PhotonSource* QEdge::get_ptn_src() const {
    return ptn_src;
}
