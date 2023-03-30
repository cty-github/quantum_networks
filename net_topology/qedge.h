//
// Created by TianyaoChu on 2023/3/29.
//

#ifndef QUANTUM_NETWORKS_QEDGE_H
#define QUANTUM_NETWORKS_QEDGE_H

class QEdge {
private:
    int node_id_a;
    int node_id_b;
    int capacity;
    int channel_occupied;
    double distance;
    double decay_rate;
    double success_probability;
public:
    QEdge(int node_id_a, int node_id_b, int capacity=10, double distance=1200, double decay_rate=0.0002);
    QEdge(const QEdge& edge);
    ~QEdge();
    bool connect_node(int node_id) const;
    int get_node_id_a() const;
    int get_node_id_b() const;
    int get_capacity() const;
    bool occupy_channel(int n_qubit);
    double get_distance() const;
    double get_decay_rate() const;
    double get_success_probability() const;
};

#endif //QUANTUM_NETWORKS_QEDGE_H
