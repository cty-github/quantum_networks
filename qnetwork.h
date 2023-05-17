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
    ClockTime start_time_point;
    ClockTime current_time_point;
    int finished_cxn_num;
    string output_filepath;
public:
    QNetwork(int ptn_src_num, int bsm_num, int user_num, int repeater_num, string output_filepath,
             double size=10000, double alpha=0.5, double beta=0.9,
             double decay_rate=0.0002, double z_fidelity=0.999, double x_fidelity=0.999);
    QNetwork(const string& net_dev_filepath, const string& net_topo_filepath,
             const string& sd_pair_filepath, string output_filepath);
    ~QNetwork();
//    bool draw_net_topo(const string& filepath) const;
    bool save_net(const string& net_dev_filepath,
                  const string& net_topo_filepath,
                  const string& sd_pair_filepath) const;
    bool initialize(int k);
    bool work_cycle(double run_time);
    int get_finished_cxn_num() const;
};

#endif //QUANTUM_NETWORKS_QNETWORK_H
