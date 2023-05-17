//
// Created by TianyaoChu on 2023/3/25.
//

#include "qnetwork.h"
#include "test_params.h"
#include "utils/rand.h"
#include <iostream>
#include <fstream>
#include <utility>

QNetwork::QNetwork(int ptn_src_num, int bsm_num, int user_num, int repeater_num,
                   double size, double alpha, double beta, string output_filepath):
start_time_point(get_current_time()), current_time_point(get_current_time()),
finished_cxn_num(0), output_filepath(std::move(output_filepath)) {
    device_manager = new DeviceManager(ptn_src_num, bsm_num, size);
    net_topo = new NetTopology(device_manager, user_num, repeater_num, size, alpha, beta);
    net_manager = new NetManager(net_topo, user_num);
}

QNetwork::QNetwork(const string& net_dev_filepath, const string& net_topo_filepath,
                   const string& sd_pair_filepath, string output_filepath):
start_time_point(get_current_time()), current_time_point(get_current_time()),
finished_cxn_num(0), output_filepath(std::move(output_filepath)) {
    device_manager = new DeviceManager(net_dev_filepath);
    net_topo = new NetTopology(net_topo_filepath, device_manager);
    net_manager = new NetManager(sd_pair_filepath, net_topo);
}

QNetwork::~QNetwork() = default;

//bool QNetwork::draw_net_topo(const string& filepath) const {
//    if (net_topo == nullptr) {
//        cout << "No Net Topology" << endl;
//        return false;
//    }
//    return false;
//}

bool QNetwork::save_net(const string& net_dev_filepath,
                        const string& net_topo_filepath,
                        const string& sd_pair_filepath) const {
    if (device_manager == nullptr) {
        cout << "No Device Manager" << endl;
        return false;
    }
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return false;
    }
    if (net_manager == nullptr) {
        cout << "No Net Manager" << endl;
        return false;
    }
    if (!device_manager->save_dev(net_dev_filepath)) {
        cout << "Save Device Fail" << endl;
        return false;
    }
    if (!net_topo->save_topo(net_topo_filepath)) {
        cout << "Save Topo Fail" << endl;
        return false;
    }
    if (!net_manager->save_sd_pairs(sd_pair_filepath)) {
        cout << "Save Request Fail" << endl;
        return false;
    }
    return true;
}

bool QNetwork::initialize(int k) {
    if (device_manager == nullptr) {
        cout << "No Device Manager" << endl;
        return false;
    }
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return false;
    }

    ofstream file;
    file.open(output_filepath,ios::out);
    if (!file.is_open()) {
        cout << "Cannot Open File " << output_filepath << endl;
        return false;
    }
    file << "#\t\treq_id\tpair_id\tcxn_id\tfide_th\t\tfide_cxn\ttask_cpl_time" << endl;
    file.close();

    cout << "--------------------------" << endl;
    cout << "PtnSrc request_num: " << device_manager->get_ptn_src_num() << endl;
    cout << "BSM request_num: " << device_manager->get_bsm_num() << endl;
    cout << "Node request_num: " << net_topo->get_node_num() << endl;
    cout << "Edge request_num: " << net_topo->get_edge_num() << endl;

    bool initialize_res = net_manager->initialize(k);
    ClockTime last_time_point = current_time_point;
    current_time_point = get_current_time();
    int time_interval = get_time_interval(current_time_point, last_time_point);
    cout << "Initialize Time: " << (double)time_interval/10000 << "s" << endl;
    cout << endl;
    return initialize_res;
}

bool QNetwork::work_cycle(double run_time) {
    ClockTime last_time_point = current_time_point;
    current_time_point = get_current_time();
    int time_interval = get_time_interval(current_time_point, last_time_point);

    cout << "--------------------------" << endl;
    cout << "Work Cycle Duration: " << time_interval << endl;
    cout << "----- Requests Phase -----" << endl;
    vector<UserRequest*> new_requests = net_manager->random_request(time_interval, TIME_PROB,
                                                                    SD_PROB, REQ_RATE);
    net_manager->add_new_requests(new_requests);
    net_manager->print_waiting_requests();
    cout << "----- Routings Phase -----" << endl;
    net_manager->schedule_new_routings();
    net_manager->print_routing_projects();
    net_manager->refresh_routing_state(time_interval);
    net_manager->print_processing_requests();
    cout << "----- Services Phase -----" << endl;
    net_manager->check_success_routing(output_filepath);
    net_manager->print_serving_requests();
    net_manager->print_user_connections();
    finished_cxn_num += net_manager->finish_user_connection(time_interval);
    cout << endl;

    if (get_time_second(current_time_point, start_time_point) > run_time) {
        return false;
    } else {
        return true;
    }
}

int QNetwork::get_finished_cxn_num() const {
    return finished_cxn_num;
}

bool QNetwork::finish() {
    cout << "--------------------------" << endl;
    cout << "Finished Connection Num: " << get_finished_cxn_num() << endl;
    cout << "Waiting Request NUM: " << net_manager->get_waiting_request_num() << endl;

    ofstream file;
    file.open(output_filepath,ios::app);
    if (!file.is_open()) {
        cout << "Cannot Open File " << output_filepath << endl;
        return false;
    }
    file << "--------------------------" << endl;
    double dist_sum = 0;
    for (auto it_edge:net_topo->get_edges()) {
        dist_sum += it_edge.second->get_distance();
    }
    double average_dist = dist_sum/net_topo->get_edge_num();
    file << "Net Average Success Rate: " << exp(-PTN_DECAY_RATE * average_dist) << endl;
    file << "Finished Connection Num: " << get_finished_cxn_num() << endl;
    file << "Waiting Request NUM: " << net_manager->get_waiting_request_num() << endl;
    file.close();
    return true;
}
