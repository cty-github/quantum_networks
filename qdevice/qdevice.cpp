//
// Created by TianyaoChu on 2023/3/25.
//

#include "qdevice.h"

int PhotonSource::qubit_num = 0;

PhotonSource::PhotonSource(int id, double pos_x, double pos_y, double decay_rate, double fidelity):
id(id), pos_x(pos_x), pos_y(pos_y), decay_rate(decay_rate), fidelity(fidelity) {}

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

double PhotonSource::get_fidelity() const {
    return fidelity;
}

BSM::BSM(int id, double pos_x, double pos_y, double fidelity, double success_rate):
id(id), pos_x(pos_x), pos_y(pos_y), z_fidelity(fidelity), x_fidelity(fidelity), success_rate(success_rate) {}

BSM::BSM(int id, double pos_x, double pos_y, double z_fidelity, double x_fidelity, double success_rate):
id(id), pos_x(pos_x), pos_y(pos_y), z_fidelity(z_fidelity), x_fidelity(x_fidelity), success_rate(success_rate) {}

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

double BSM::get_success_rate() const {
    return success_rate;
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
