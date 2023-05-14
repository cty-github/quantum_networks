//
// Created by TianyaoChu on 2023/5/7.
//

#include "entangle_level.h"
#include "utils/rand.h"
#include <iostream>

//EntangleLink::EntangleLink(int node_id_a, int node_id_b,
//                           double src_bit_flip, double dst_bit_flip,
//                           double src_phase_flip, double dst_phase_flip):
//node_id_a(node_id_a), node_id_b(node_id_b),
//x_fidelity(src_bit_flip*dst_bit_flip+(1-src_bit_flip)*(1-dst_bit_flip)),
//z_fidelity(src_phase_flip*dst_phase_flip+(1-src_phase_flip)*(1-dst_phase_flip)), age(0) {}

EntangleLink::EntangleLink(int node_id_a, int node_id_b, double x_fidelity, double z_fidelity):
node_id_a(node_id_a), node_id_b(node_id_b), x_fidelity(x_fidelity), z_fidelity(z_fidelity), age(0) {}

EntangleLink::~EntangleLink() = default;

int EntangleLink::get_node_id_a() const {
    return node_id_a;
}

int EntangleLink::get_node_id_b() const {
    return node_id_b;
}

double EntangleLink::get_x_fidelity() const {
    return x_fidelity;
}

double EntangleLink::get_z_fidelity() const {
    return z_fidelity;
}

double EntangleLink::get_fidelity() const {
    return z_fidelity * x_fidelity;
}

int EntangleLink::get_age() const {
    return age;
}

void EntangleLink::add_age(int time) {
    age += time;
}

bool EntangleLink::is_expired() const {
    return age > LIFESPAN;
}

EntangleLink* purify_links(vector<EntangleLink*> links) {
    return links.front();
}

EntangleLink* swap_link(EntangleLink* prev_link, EntangleLink* next_link, QNode* node) {
    uniform_real_distribution<double> rand_double(0.0,1.0);
    if (rand_double(rand_eng) > node->get_success_rate()) {
        delete prev_link;
        delete next_link;
        return nullptr;
    }
    int node_id_a;
    if (prev_link->get_node_id_a() == node->get_id()) {
        node_id_a = prev_link->get_node_id_b();
    } else {
        node_id_a = prev_link->get_node_id_a();
    }
    int node_id_b;
    if (next_link->get_node_id_a() == node->get_id()) {
        node_id_b = next_link->get_node_id_b();
    } else {
        node_id_b = next_link->get_node_id_a();
    }
    BSM* bsm = node->get_bsm();
    double prev_x_f = prev_link->get_x_fidelity();
    double next_x_f = next_link->get_x_fidelity();
    double x_fidelity = prev_x_f * next_x_f + (1-prev_x_f) * (1-next_x_f);
    x_fidelity = x_fidelity * bsm->get_x_fidelity() + (1-x_fidelity) * (1-bsm->get_x_fidelity());
    double prev_z_f = prev_link->get_z_fidelity();
    double next_z_f = next_link->get_z_fidelity();
    double z_fidelity = prev_z_f * next_z_f + (1-prev_z_f) * (1-next_z_f);
    z_fidelity = z_fidelity * bsm->get_z_fidelity() + (1-z_fidelity) * (1-bsm->get_z_fidelity());
    auto* new_link = new EntangleLink(node_id_a, node_id_b, x_fidelity, z_fidelity);
    delete prev_link;
    delete next_link;
    return new_link;
}

EntangleSegment::EntangleSegment(EntangleLink* etg_link, int s_id, int d_id):
etg_link(etg_link), s_id(s_id), d_id(d_id) {}

EntangleSegment::EntangleSegment(EntangleSegment *etg_seg):
etg_link(etg_seg->etg_link), s_id(etg_seg->s_id), d_id(etg_seg->d_id) {}

EntangleSegment::~EntangleSegment() = default;

EntangleLink* EntangleSegment::get_etg_link() const {
    return etg_link;
}

int EntangleSegment::get_s_id() const {
    return s_id;
}

int EntangleSegment::get_d_id() const {
    return d_id;
}

double EntangleSegment::get_x_fidelity() const {
    return etg_link->get_x_fidelity();
}

double EntangleSegment::get_z_fidelity() const {
    return etg_link->get_z_fidelity();
}

double EntangleSegment::get_fidelity() const {
    return etg_link->get_fidelity();
}

int EntangleSegment::get_age() const {
    return etg_link->get_age();
}

void EntangleSegment::add_age(int time) {
    etg_link->add_age(time);
}

bool EntangleSegment::is_expired() const {
    return etg_link->is_expired();
}

EntangleConnection::EntangleConnection(EntangleSegment* etg_seg): EntangleSegment(etg_seg) {}

EntangleConnection::~EntangleConnection() = default;

void EntangleConnection::print_etg_cxn() const {
    cout << "Entangle Connection: Node " << get_s_id() << " <-> Node " << get_d_id() << endl;
    cout << "X Fidelity: " << get_x_fidelity() << endl;
    cout << "Z Fidelity: " << get_z_fidelity() << endl;
    cout << "Total Fidelity: " << get_fidelity() << endl;
}
