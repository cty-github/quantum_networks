//
// Created by TianyaoChu on 2023/3/25.
//

#ifndef QUANTUM_NETWORKS_QDEVICE_H
#define QUANTUM_NETWORKS_QDEVICE_H

//#include "qelement.h"

class EntangleLink {
private:
    int src_node_id;
    int dst_node_id;
    double src_p_flip;
    double dst_p_flip;
    double x_fidelity;
    double z_fidelity;
public:
    EntangleLink(int src_node_id, int dst_node_id,
                 double src_p_flip, double dst_p_flip,
                 double x_fidelity=1, double z_fidelity=1);
    ~EntangleLink();
    int get_src_id() const;
    int get_dst_id() const;
    double get_src_p() const;
    double get_dst_p() const;
    double get_x_fidelity() const;
    double get_z_fidelity() const;
    double get_bit_fidelity() const;
    double get_fidelity() const;
};

class PhotonSource {
private:
    int id;
    double pos_x;
    double pos_y;
    double decay_rate;
    static int qubit_num;
public:
    PhotonSource(int id, double pos_x, double pos_y, double decay_rate);
    ~PhotonSource();
    int get_id() const;
    double get_pos_x() const;
    double get_pos_y() const;
    double get_decay_rate() const;
//    QReg generate_pair(int node_id_a, int node_id_b, double dist_a, double dist_b) const;
    EntangleLink* generate_link(int node_id_a, int node_id_b, double dist_a, double dist_b) const;
};

//class BellRes {
//public:
//    int pauli_z;
//    int pauli_x;
//    BellRes(int pauli_z, int pauli_x);
//    ~BellRes();
//    BellRes& operator= (const BellRes& other) = default;
//    BellRes operator+ (const BellRes& other) const {
//        return {(pauli_z+other.pauli_z)%2, (pauli_x+other.pauli_x)%2};
//    }
//};

class BSM {
private:
    int id;
    double pos_x;
    double pos_y;
    double z_fidelity;
    double x_fidelity;
public:
    BSM(int id, double pos_x, double pos_y, double fidelity);
    BSM(int id, double pos_x, double pos_y, double z_fidelity, double x_fidelity);
    ~BSM();
    int get_id() const;
    double get_pos_x() const;
    double get_pos_y() const;
    double get_z_fidelity() const;
    double get_x_fidelity() const;
//    BellRes internal_measure(QReg* qreg, int node_id) const;
    EntangleLink* connect_link(EntangleLink* src_link, EntangleLink* dst_link) const;
};

//class EntangleRoute {
//private:
//    QReg entangle_connect;
//    Routing* route;
//    NetTopology* net_topo;
//    PhotonSource* ptn_src;
//    BSM* bsm;
//public:
//    EntangleRoute(Routing* route, NetTopology* net_topo, PhotonSource* ptn_src, BSM* bsm);
//    ~EntangleRoute();
//    QReg* build_connect();
//    bool test_connect();
//};

#endif //QUANTUM_NETWORKS_QDEVICE_H
