//
// Created by TianyaoChu on 2023/3/29.
//

#ifndef QUANTUM_NETWORKS_QEDGE_H
#define QUANTUM_NETWORKS_QEDGE_H

#include "qdevice/qdevice.h"

class QEdge {
private:
    int edge_id;
    int node_id_a;
    int node_id_b;
    int channel_capacity;
    double distance;
    double success_rate;
    PhotonSource* ptn_src;
public:
    QEdge(int edge_id, int node_id_a, int node_id_b, int channel_capacity,
          double distance, double success_rate, PhotonSource* ptn_src);
    QEdge(const QEdge& edge);
    ~QEdge();
    bool connect_node(int node_id) const;
    int get_edge_id() const;
    int get_node_id_a() const;
    int get_node_id_b() const;
    int get_channel_capacity() const;
    double get_distance() const;
    double get_success_rate() const;
    PhotonSource* get_ptn_src() const;
};

#endif //QUANTUM_NETWORKS_QEDGE_H
