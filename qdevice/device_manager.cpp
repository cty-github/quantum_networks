//
// Created by TianyaoChu on 2023/3/30.
//

#include "device_manager.h"
#include "utils/rand.h"
#include "utils/tool.h"
#include <iostream>

DeviceManager::DeviceManager(): ptn_src_num(0), bsm_num(0) {}

DeviceManager::DeviceManager(int ptn_src_num, int bsm_num,
                             double size, double decay_rate,
                             double z_fidelity, double x_fidelity):
ptn_src_num(0), bsm_num(0) {
    uniform_real_distribution<double> rand_double(0.0,1.0);
    for (int i = 0; i < ptn_src_num; i++) {
        add_ptn_src(i, size*rand_double(rand_eng), size*rand_double(rand_eng), decay_rate);
    }
    for (int i = 0; i < bsm_num; i++) {
        add_bsm(i,
                size*rand_double(rand_eng),
                size*rand_double(rand_eng),
                z_fidelity, x_fidelity);
    }
}

DeviceManager::~DeviceManager() = default;

int DeviceManager::get_ptn_src_num() const {
    return ptn_src_num;
}

int DeviceManager::get_bsm_num() const {
    return bsm_num;
}

map<int, PhotonSource*> DeviceManager::get_ptn_src_list() const {
    return ptn_src_list;
}

map<int, BSM*> DeviceManager::get_bsm_list() const {
    return bsm_list;
}

PhotonSource* DeviceManager::get_ptn_src(int id) const {
    if (ptn_src_list.find(id) == ptn_src_list.end()) {
        cout << "No Photon Source " << id << endl;
        return nullptr;
    }
    return ptn_src_list.find(id)->second;
}

BSM* DeviceManager::get_bsm(int id) const {
    if (bsm_list.find(id) == bsm_list.end()) {
        cout << "No BSM " << id << endl;
        return nullptr;
    }
    return bsm_list.find(id)->second;
}

bool DeviceManager::add_ptn_src(int id, double pos_x, double pos_y, double decay_rate) {
    if (ptn_src_list.find(id) != ptn_src_list.end()) {
        cout << "Exist Photon Source " << id << endl;
        return false;
    }
    auto* new_ptn_src = new PhotonSource(id, pos_x, pos_y, decay_rate);
    ptn_src_list[id] = new_ptn_src;
    ptn_src_num++;
    return true;
}

bool DeviceManager::add_bsm(int id, double pos_x, double pos_y, double z_fidelity, double x_fidelity) {
    if (bsm_list.find(id) != bsm_list.end()) {
        cout << "Exist BSM " << id << endl;
        return false;
    }
    auto* new_bsm = new BSM(id, pos_x, pos_y, z_fidelity, x_fidelity);
    bsm_list[id] = new_bsm;
    bsm_num++;
    return true;
}

PhotonSource* DeviceManager::get_closest_ptn_src(double x, double y) {
    PhotonSource* ptn_src = nullptr;
    double min_dist;
    for (auto it:ptn_src_list) {
        if (!ptn_src) {
            ptn_src = it.second;
            min_dist = dist(x, y, ptn_src->get_pos_x(), ptn_src->get_pos_y());
        } else {
            if (dist(x, y, it.second->get_pos_x(), it.second->get_pos_y()) < min_dist) {
                ptn_src = it.second;
                min_dist = dist(x, y, ptn_src->get_pos_x(), ptn_src->get_pos_y());
            }
        }
    }
    return ptn_src;
}

BSM* DeviceManager::get_closest_bsm(double x, double y) {
    BSM* bsm = nullptr;
    double min_dist;
    for (auto it:bsm_list) {
        if (!bsm) {
            bsm = it.second;
            min_dist = dist(x, y, bsm->get_pos_x(), bsm->get_pos_y());
        } else {
            if (dist(x, y, it.second->get_pos_x(), it.second->get_pos_y()) < min_dist) {
                bsm = it.second;
                min_dist = dist(x, y, bsm->get_pos_x(), bsm->get_pos_y());
            }
        }
    }
    return bsm;
}
