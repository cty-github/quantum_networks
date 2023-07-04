//
// Created by TianyaoChu on 2023/3/29.
//

#include "qedge.h"

QEdge::QEdge(int edge_id, int node_id_a, int node_id_b, int channel_capacity,
             double distance, double success_rate, PhotonSource* ptn_src):
edge_id(edge_id), node_id_a(node_id_a), node_id_b(node_id_b),
channel_capacity(channel_capacity), distance(distance), success_rate(success_rate), ptn_src(ptn_src) {}

QEdge::QEdge(const QEdge& edge) {
    edge_id = edge.edge_id;
    node_id_a = edge.node_id_a;
    node_id_b = edge.node_id_b;
    channel_capacity = edge.channel_capacity;
    distance = edge.distance;
    success_rate = edge.success_rate;
    ptn_src = edge.ptn_src;
}

QEdge::~QEdge() = default;

bool QEdge::connect_node(int node_id) const {
    return node_id_a == node_id || node_id_b == node_id;
}

int QEdge::get_edge_id() const {
    return edge_id;
}

int QEdge::get_node_id_a() const {
    return node_id_a;
}

int QEdge::get_node_id_b() const {
    return node_id_b;
}

int QEdge::get_channel_capacity() const {
    return channel_capacity;
}

double QEdge::get_distance() const {
    return distance;
}

double QEdge::get_success_rate() const {
    return success_rate;
}

PhotonSource* QEdge::get_ptn_src() const {
    return ptn_src;
}
