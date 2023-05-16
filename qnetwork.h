//
// Created by TianyaoChu on 2023/3/25.
//

#ifndef QUANTUM_NETWORKS_QNETWORK_H
#define QUANTUM_NETWORKS_QNETWORK_H

#include "net_topology/net_topology.h"
#include "qdevice/device_manager.h"
#include "net_manager/net_manager.h"
#include "utils/timing.h"
#include <tuple>

class QNetwork {
private:
    DeviceManager* device_manager;
    NetTopology* net_topo;
    NetManager* net_manager;
    ClockTime current_time_point;
public:
    QNetwork(int ptn_src_num, int bsm_num,
             int user_num, int repeater_num,
             double size=10000, double alpha=0.5, double beta=0.9,
             double decay_rate=0.0015, double z_fidelity=0.999, double x_fidelity=0.999);
    QNetwork(const string& net_dev_filepath, const string& net_topo_filepath, const string& sd_pair_filepath);
    ~QNetwork();
    int get_node_num() const;
    int get_edge_num() const;
//    bool draw_net_topo(const string& filepath) const;
    bool save_net(const string& net_dev_filepath,
                  const string& net_topo_filepath,
                  const string& sd_pair_filepath) const;
    bool initialize(int k);
    bool work_cycle();
};

#endif //QUANTUM_NETWORKS_QNETWORK_H
