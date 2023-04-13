//
// Created by TianyaoChu on 2023/3/25.
//

#include "qdevice.h"
#include <cmath>
#include <iostream>

using namespace std;

EntangleLink::EntangleLink(int src_node_id, int dst_node_id,
                           double src_p_flip, double dst_p_flip,
                           double x_fidelity, double z_fidelity):
src_node_id(src_node_id), dst_node_id(dst_node_id),
src_p_flip(src_p_flip), dst_p_flip(dst_p_flip),
x_fidelity(x_fidelity), z_fidelity(z_fidelity) {}

EntangleLink::~EntangleLink() = default;

int EntangleLink::get_src_id() const {
    return src_node_id;
}

int EntangleLink::get_dst_id() const {
    return dst_node_id;
}

double EntangleLink::get_src_p() const {
    return src_p_flip;
}

double EntangleLink::get_dst_p() const {
    return dst_p_flip;
}

double EntangleLink::get_x_fidelity() const {
    return x_fidelity;
}

double EntangleLink::get_z_fidelity() const {
    return z_fidelity;
}

int PhotonSource::qubit_num = 0;

PhotonSource::PhotonSource(int id, double pos_x, double pos_y, double decay_rate):
id(id), pos_x(pos_x), pos_y(pos_y), decay_rate(decay_rate) {}

PhotonSource::~PhotonSource() = default;

int PhotonSource::get_id() const {
    return id;
}

double PhotonSource::get_pos_x() const {
    return pos_x;
}

double PhotonSource::get_pos_y() const {
    return pos_y;
}

double PhotonSource::get_decay_rate() const {
    return decay_rate;
}

//QReg PhotonSource::generate_pair(int node_id_a, int node_id_b, double dist_a, double dist_b) const {
//    double p_a = exp(-decay_rate*dist_a);
//    double p_b = exp(-decay_rate*dist_b);
//    auto* qubit_a = new Qubit(qubit_num, node_id_a, Zero, X, p_a);
//    qubit_num++;
//    auto* qubit_b = new Qubit(qubit_num, node_id_b, Zero, Z, p_b);
//    qubit_num++;
//    vector<Qubit*> qubits = {qubit_a, qubit_b};
//    QReg qreg(qubits);
//    qreg.hadamard(0);
//    qreg.cnot(0, 1);
//    return qreg;
//}

EntangleLink* PhotonSource::generate_link(int node_id_a, int node_id_b, double dist_a, double dist_b) const {
    qubit_num += 2;
    double p_a = exp(-decay_rate*dist_a);
    double p_b = exp(-decay_rate*dist_b);
    return new EntangleLink(node_id_a, node_id_b, p_a, p_b);
}

//BellRes::BellRes(int pauli_z, int pauli_x): pauli_z(pauli_z), pauli_x(pauli_x) {}
//
//BellRes::~BellRes() = default;

BSM::BSM(int id, double pos_x, double pos_y, double fidelity):
id(id), pos_x(pos_x), pos_y(pos_y), z_fidelity(fidelity), x_fidelity(fidelity) {}

BSM::BSM(int id, double pos_x, double pos_y, double z_fidelity, double x_fidelity):
id(id), pos_x(pos_x), pos_y(pos_y), z_fidelity(z_fidelity), x_fidelity(x_fidelity) {}

BSM::~BSM() = default;

int BSM::get_id() const {
    return id;
}

double BSM::get_pos_x() const {
    return pos_x;
}

double BSM::get_pos_y() const {
    return pos_y;
}

double BSM::get_z_fidelity() const {
    return z_fidelity;
}

double BSM::get_x_fidelity() const {
    return x_fidelity;
}

//BellRes BSM::internal_measure(QReg* qreg, int node_id) const {
//    int z_id = qreg->get_qubit(node_id, Z);
//    int x_id = qreg->get_qubit(node_id, X);
//    int z_index = qreg->get_r_index(z_id);
//    int x_index = qreg->get_r_index(x_id);
//    qreg->cnot(z_index, x_index);
//    qreg->hadamard(z_index);
//    Qubit* qubit_z = qreg->measure(z_id, z_fidelity);
//    Qubit* qubit_x = qreg->measure(x_id, x_fidelity);
//    return {qubit_z->get_value(), qubit_x->get_value()};
//}

EntangleLink* BSM::connect_link(EntangleLink* src_link, EntangleLink* dst_link) const {
    if (src_link->get_dst_id() != dst_link->get_src_id()) {
        cout << "Error: Not the Same Repeater" << endl;
        return nullptr;
    }
    int new_src_id = src_link->get_src_id();
    int new_dst_id = dst_link->get_dst_id();
    double new_src_p = src_link->get_src_p();
    double new_dst_p = dst_link->get_dst_p();

    double p_flip_z = src_link->get_dst_p();
    double p_flip_x = dst_link->get_src_p();
    double src_x_f = src_link->get_x_fidelity();
    double src_z_f = src_link->get_z_fidelity();
    double dst_x_f = dst_link->get_x_fidelity();
    double dst_z_f = dst_link->get_z_fidelity();

    double bit_fidelity = p_flip_z * p_flip_x + (1-p_flip_z) * (1-p_flip_x);
    double new_x_fidelity = src_x_f * dst_x_f + (1-src_x_f) * (1-dst_x_f);
    new_x_fidelity = new_x_fidelity * x_fidelity + (1-new_x_fidelity) * (1-x_fidelity);
    new_x_fidelity = new_x_fidelity * bit_fidelity + (1-new_x_fidelity) * (1-bit_fidelity);
    double new_z_fidelity = src_z_f * dst_z_f + (1-src_z_f) * (1-dst_z_f);
    new_z_fidelity = new_z_fidelity * z_fidelity + (1-new_z_fidelity) * (1-z_fidelity);

    return new EntangleLink(new_src_id, new_dst_id,
                            new_src_p, new_dst_p,
                            new_x_fidelity, new_z_fidelity);
}

//EntangleRoute::EntangleRoute(Routing* route, NetTopology* net_topo, PhotonSource* ptn_src, BSM* bsm):
//        entangle_connect(), net_topo(net_topo), route(route), ptn_src(ptn_src), bsm(bsm) {}
//
//EntangleRoute::~EntangleRoute() = default;
//
//QReg* EntangleRoute::build_connect() {
//    for (auto node:route->get_path()) {
//        int node_id_a = node->get_id();
//        if (node_id_a == route->get_end_node_id()) {
//            continue;
//        }
//        int node_id_b = route->get_next_node_id(node_id_a);
//        double dist = net_topo->get_edge(node_id_a, node_id_b)->get_distance();
//        QReg entangle_link = ptn_src->generate_pair(node_id_a, node_id_b, dist/2, dist/2);
//        entangle_connect.tensor_product(entangle_link);
//    }
//    BellRes bell_res(0, 0);
//    for (auto node:route->get_path()) {
//        int node_id = node->get_id();
//        if (node_id == route->get_start_node_id() || node_id == route->get_end_node_id()) {
//            continue;
//        }
//        bell_res = bell_res + bsm->internal_measure(&entangle_connect, node_id);
//    }
//    int dst_node_id = route->get_end_node_id();
//    int dst_q_id = entangle_connect.get_qubit(dst_node_id, Z);
//    int dst_r_index = entangle_connect.get_r_index(dst_q_id);
//    if (bell_res.pauli_x) {
//        entangle_connect.pauli_x(dst_r_index);
//    }
//    if (bell_res.pauli_z) {
//        entangle_connect.pauli_z(dst_r_index);
//    }
//    return &entangle_connect;
//}
//
//bool EntangleRoute::test_connect() {}
