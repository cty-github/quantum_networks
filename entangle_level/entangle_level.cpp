//
// Created by TianyaoChu on 2023/5/7.
//

#include "entangle_level.h"
#include <iostream>

EntangleLink::~EntangleLink() = default;

EntangleConnection::~EntangleConnection() = default;

EntangleLink::EntangleLink(int src_node_id, int dst_node_id,
                           double src_bit_flip, double dst_bit_flip,
                           double src_phase_flip, double dst_phase_flip):
src_node_id(src_node_id), dst_node_id(dst_node_id),
x_fidelity(src_bit_flip*dst_bit_flip+(1-src_bit_flip)*(1-dst_bit_flip)),
z_fidelity(src_phase_flip*dst_phase_flip+(1-src_phase_flip)*(1-dst_phase_flip)) {}

EntangleLink::EntangleLink(int src_node_id, int dst_node_id,
                           double x_fidelity, double z_fidelity):
src_node_id(src_node_id), dst_node_id(dst_node_id),
x_fidelity(x_fidelity), z_fidelity(z_fidelity) {}

int EntangleLink::get_src_id() const {
    return src_node_id;
}

int EntangleLink::get_dst_id() const {
    return dst_node_id;
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

EntangleConnection::EntangleConnection(EntangleLink* etg_link, Path* path, NetTopology* net_topo):
        etg_link(etg_link), path(path), net_topo(net_topo) {}

EntangleLink* EntangleConnection::get_etg_link() const {
    return etg_link;
}

EntangleSegment::EntangleSegment() {}

EntangleSegment::~EntangleSegment() = default;

void EntangleConnection::print_etg_link() const {
    cout << "Entangle Connection: Node " << etg_link->get_src_id() << " <-> Node " << etg_link->get_dst_id() << endl;
    cout << "X Fidelity: " << etg_link->get_x_fidelity() << endl;
    cout << "Z Fidelity: " << etg_link->get_z_fidelity() << endl;
    cout << "Total Fidelity: " << etg_link->get_fidelity() << endl;
}

Path* EntangleConnection::get_path() const {
    return path;
}

NetTopology* EntangleConnection::get_net_topo() const {
    return net_topo;
}
