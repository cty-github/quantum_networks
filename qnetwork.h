//
// Created by TianyaoChu on 2023/3/25.
//

#ifndef QUANTUM_NETWORKS_QNETWORK_H
#define QUANTUM_NETWORKS_QNETWORK_H

#include "net_topology/net_topology.h"
#include "qdevice/device_manager.h"
#include "net_manager/net_manager.h"

class QNetwork {
private:
    DeviceManager* device_manager{};
    NetTopology* net_topology{};
    LinkManager* link_mgr{};
public:
    QNetwork();
    QNetwork(int ptn_src_num, int bsm_num,
             int user_num, int repeater_num,
             double size=10000, double alpha=0.5, double beta=0.9,
             double decay_rate=0.0002, double z_fidelity=0.9, double x_fidelity=0.9);
    explicit QNetwork(const string& net_topo_filepath);
    ~QNetwork();
    int get_node_num() const;
    int get_edge_num() const;
    bool save_net_topo(const string& filepath) const;
    Routing* get_routing(int src_node_id, int dst_node_id);
    vector<Routing*> get_routings(int src_node_id, int dst_node_id, int k);
    EntangleRoute* generate_etg_route(Routing* route);
};

#endif //QUANTUM_NETWORKS_QNETWORK_H
