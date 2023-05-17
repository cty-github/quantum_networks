//
// Created by TianyaoChu on 2023/3/30.
//

#ifndef QUANTUM_NETWORKS_DEVICE_MANAGER_H
#define QUANTUM_NETWORKS_DEVICE_MANAGER_H

#include "qdevice.h"
#include <string>
#include <map>

using namespace std;

class DeviceManager {
    int ptn_src_num;
    int bsm_num;
    map<int, PhotonSource*> ptn_src_list;
    map<int, BSM*> bsm_list;
public:
    DeviceManager();
    DeviceManager(int ptn_src_num, int bsm_num, double net_size);
    explicit DeviceManager(const string& filepath);
    ~DeviceManager();
    int get_ptn_src_num() const;
    int get_bsm_num() const;
    map<int, PhotonSource*> get_ptn_src_list() const;
    map<int, BSM*> get_bsm_list() const;
    PhotonSource* get_ptn_src(int id) const;
    BSM* get_bsm(int id) const;
    bool add_ptn_src(int id, double pos_x, double pos_y, double decay_rate, double fidelity);
    bool add_bsm(int id, double pos_x, double pos_y, double z_fidelity, double x_fidelity, double success_rate);
    bool save_dev(const string& filepath) const;
    PhotonSource* get_closest_ptn_src(double x, double y);
    BSM* get_closest_bsm(double x, double y);
};

#endif //QUANTUM_NETWORKS_DEVICE_MANAGER_H
