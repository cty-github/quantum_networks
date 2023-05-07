//
// Created by TianyaoChu on 2023/5/7.
//

#ifndef QUANTUM_NETWORKS_ENTANGLE_LEVEL_H
#define QUANTUM_NETWORKS_ENTANGLE_LEVEL_H

//#include "qelement.h"
#include "net_topology/net_topology.h"
#include "net_topology/path.h"

class EntangleLink {
private:
    int src_node_id;
    int dst_node_id;
    double x_fidelity;
    double z_fidelity;
public:
    EntangleLink(int src_node_id, int dst_node_id,
                 double src_bit_flip, double dst_bit_flip,
                 double src_phase_flip, double dst_phase_flip);
    EntangleLink(int src_node_id, int dst_node_id,
                 double x_fidelity, double z_fidelity);
    ~EntangleLink();
    int get_src_id() const;
    int get_dst_id() const;
    double get_x_fidelity() const;
    double get_z_fidelity() const;
    double get_fidelity() const;
};

class EntangleSegment {
private:
    EntangleLink* etg_link{};
    Path* path{};
    NetTopology* net_topo{};
    int s_id;
    int d_id;
public:
    EntangleSegment();
    ~EntangleSegment();
};

class EntangleConnection {
private:
    EntangleLink* etg_link{};
    Path* path{};
    NetTopology* net_topo{};
    int s_id;
    int d_id;
public:
    EntangleConnection(EntangleLink* etg_link, Path* path, NetTopology* net_topo);
    ~EntangleConnection();
    EntangleLink* get_etg_link() const;
    void print_etg_link() const;
    Path* get_path() const;
    NetTopology* get_net_topo() const;
};

#endif //QUANTUM_NETWORKS_ENTANGLE_LEVEL_H
