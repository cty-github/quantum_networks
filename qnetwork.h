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
    ClockTime sample_time_point;
    ClockTime current_time_point;
    int sample_route_num;
    int sample_cxn_num;
    int finished_route_num;
    int finished_cxn_num;
    string runtime_filepath;
    string metric_filepath;
public:
    QNetwork(int ptn_src_num, int bsm_num, int user_num, int repeater_num,
             double size, double alpha, double beta, const string& runtime_filepath, string metric_filepath);
    QNetwork(const string& net_dev_filepath, const string& net_topo_filepath,
             const string& sd_pair_filepath, const string& runtime_filepath, string metric_filepath);
    ~QNetwork();
//    bool draw_net_topo(const string& filepath) const;
    bool save_net(const string& net_dev_filepath,
                  const string& net_topo_filepath,
                  const string& sd_pair_filepath) const;
    bool initialize(int k);
    bool work_cycle(double run_time);
    bool sample_cycle(int time_interval, int cycle_finish_route, int cycle_finish_cxn);
    bool finish();
};

#endif //QUANTUM_NETWORKS_QNETWORK_H
