//
// Created by TianyaoChu on 2023/3/25.
//

#ifndef QUANTUM_NETWORKS_QDEVICE_H
#define QUANTUM_NETWORKS_QDEVICE_H

class PhotonSource {
private:
    int id;
    double pos_x;
    double pos_y;
    double decay_rate;
    double fidelity;
public:
    static int qubit_num;
    PhotonSource(int id, double pos_x, double pos_y, double decay_rate, double fidelity);
    ~PhotonSource();
    int get_id() const;
    double get_pos_x() const;
    double get_pos_y() const;
    double get_decay_rate() const;
    double get_fidelity() const;
};

class BSM {
private:
    int id;
    double pos_x;
    double pos_y;
    double z_fidelity;
    double x_fidelity;
    double success_rate;
public:
    BSM(int id, double pos_x, double pos_y, double fidelity, double success_rate);
    BSM(int id, double pos_x, double pos_y, double z_fidelity, double x_fidelity, double success_rate);
    ~BSM();
    int get_id() const;
    double get_pos_x() const;
    double get_pos_y() const;
    double get_z_fidelity() const;
    double get_x_fidelity() const;
    double get_success_rate() const;
//    BellRes internal_measure(QReg* qreg, int node_id) const;
};

#endif //QUANTUM_NETWORKS_QDEVICE_H
