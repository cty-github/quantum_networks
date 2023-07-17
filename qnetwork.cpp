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
                   double size, double alpha, double beta,
                   const string& param_filepath, string runtime_filepath, string metric_filepath):
start_time_point(get_current_time()), sample_time_point(get_current_time()), current_time_point(get_current_time()),
sample_route_num(0), sample_cxn_num(0), finished_route_num(0), finished_cxn_num(0),
runtime_filepath(std::move(runtime_filepath)), metric_filepath(std::move(metric_filepath)) {
    device_manager = new DeviceManager(ptn_src_num, bsm_num, size);
    net_topo = new NetTopology(device_manager, user_num, repeater_num, size, alpha, beta);
    net_manager = new NetManager(net_topo, user_num);
    ofstream param_file;
    param_file.open(param_filepath, ios::out);
    if (!param_file.is_open()) {
        throw logic_error("Cannot Open File " + param_filepath);
    }
    param_file << "------ NetTopology ------" << endl;
    param_file << "Net Size: " << NET_SIZE << endl;
    param_file << "User Num: " << USER_NUM << endl;
    param_file << "Repeater Num: " << REPEATER_NUM << endl;
    param_file << "Mem Lower Bound: " << MEM_LOW << endl;
    param_file << "Mem Upper Bound: " << MEM_UP << endl;
    param_file << "Edge Num: " << net_topo->get_edge_num() << endl;
    double dist_sum = 0;
    for (auto it_edge:net_topo->get_edges()) {
        dist_sum += it_edge.second->get_distance();
    }
    double average_dist = dist_sum/net_topo->get_edge_num();
    param_file << "Net Average Edge Length: " << average_dist << endl;
    param_file << "Net Average Success Rate: " << exp(-PTN_DECAY_RATE * average_dist) << endl;
    param_file << "Cap Lower Bound: " << CAP_LOW << endl;
    param_file << "Cap Upper Bound: " << CAP_UP << endl;
    param_file << "SD Num: " << net_manager->get_sd_num() << endl;
    param_file << "Decay Rate: " << PTN_DECAY_RATE << endl;
    param_file << endl;
    param_file.close();
}

QNetwork::QNetwork(const string& net_dev_filepath, const string& net_topo_filepath, const string& sd_pair_filepath,
                   string runtime_filepath, string metric_filepath):
start_time_point(get_current_time()), sample_time_point(get_current_time()), current_time_point(get_current_time()),
sample_route_num(0), sample_cxn_num(0), finished_route_num(0), finished_cxn_num(0),
runtime_filepath(std::move(runtime_filepath)), metric_filepath(std::move(metric_filepath)) {
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

//    cout << "--------------------------" << endl;
//    cout << "PtnSrc Num: " << device_manager->get_ptn_src_num() << endl;
//    cout << "BSM Num: " << device_manager->get_bsm_num() << endl;
//    cout << "Node Num: " << net_topo->get_node_num() << endl;
//    cout << "Edge Num: " << net_topo->get_edge_num() << endl;

    bool initialize_res = net_manager->initialize(k);
    current_time_point = get_current_time();
    cout << "Initialized with " << get_time_second(current_time_point, start_time_point) << "s" << endl;
    return initialize_res;
}

bool QNetwork::dynamic_cycle() {
    ClockTime last_time_point = current_time_point;
    current_time_point = get_current_time();
    int time_interval = get_time_interval(current_time_point, last_time_point);

    //  ----- Requests Phase -----
    vector<UserRequest*> new_requests = net_manager->random_request(time_interval, TIME_PROB,
                                                                    SD_PROB, REQ_RATE);
    net_manager->add_new_requests(new_requests);
//    net_manager->print_waiting_requests();

    //  ----- Routings Phase -----
    net_manager->schedule_new_routings();
//    net_manager->print_routing_projects();
    net_manager->refresh_routing_state(time_interval);
//    net_manager->print_processing_requests();

    //  ----- Services Phase -----
    int cycle_finish_route = net_manager->check_success_routing(runtime_filepath);
    net_manager->check_killed_routing(runtime_filepath);
    sample_route_num += cycle_finish_route;
    finished_route_num += cycle_finish_route;
//    net_manager->print_serving_requests();
//    net_manager->print_user_connections();
    int cycle_finish_cxn = net_manager->finish_user_connection(time_interval);
    sample_cxn_num += cycle_finish_cxn;
    finished_cxn_num += cycle_finish_cxn;
//    cout << endl;

    int sample_time_interval = get_time_interval(current_time_point, sample_time_point);
    if (get_time_second(current_time_point, start_time_point) > RUN_TIME) {
        sample_cycle(sample_time_interval, sample_route_num, sample_cxn_num);
        sample_route_num = 0;
        sample_cxn_num = 0;
        return false;
    } else if (sample_time_interval > SAMPLE_INT) {
        sample_cycle(sample_time_interval, sample_route_num, sample_cxn_num);
        sample_time_point = current_time_point;
        sample_route_num = 0;
        sample_cxn_num = 0;
        return true;
    } else {
        return true;
    }
}

bool QNetwork::static_cycle() {
    ClockTime last_time_point = current_time_point;
    current_time_point = get_current_time();
    int time_interval = get_time_interval(current_time_point, last_time_point);

    //  ----- Requests Phase -----
    vector<UserRequest*> new_requests = net_manager->random_request(time_interval, TIME_PROB,
                                                                    SD_PROB, REQ_RATE);
    net_manager->add_new_requests(new_requests);

    //  ----- Routings Phase -----
    int left = net_manager -> get_leftover_routes();
    if(left == 0){
        net_manager->static_schedule_new_routings();
    }
    net_manager->refresh_routing_state(time_interval);

    //  ----- Services Phase -----
    int cycle_finish_route = net_manager->check_success_routing(runtime_filepath);
    net_manager->check_killed_routing(runtime_filepath);
    sample_route_num += cycle_finish_route;
    finished_route_num += cycle_finish_route;
//    net_manager->print_serving_requests();
//    net_manager->print_user_connections();
    int cycle_finish_cxn = net_manager->finish_user_connection(time_interval);
    sample_cxn_num += cycle_finish_cxn;
    finished_cxn_num += cycle_finish_cxn;
//    cout << endl;

    int sample_time_interval = get_time_interval(current_time_point, sample_time_point);
    if (get_time_second(current_time_point, start_time_point) > RUN_TIME) {
        sample_cycle(sample_time_interval, sample_route_num, sample_cxn_num);
        sample_route_num = 0;
        sample_cxn_num = 0;
        return false;
    } else if (sample_time_interval > SAMPLE_INT) {
        sample_cycle(sample_time_interval, sample_route_num, sample_cxn_num);
        sample_time_point = current_time_point;
        sample_route_num = 0;
        sample_cxn_num = 0;
        return true;
    } else {
        return true;
    }
}

bool QNetwork::sample_cycle(int time_interval, int cycle_finish_route, int cycle_finish_cxn) {
    ofstream runtime_file;
    runtime_file.open(runtime_filepath, ios::app);
    if (!runtime_file.is_open()) {
        throw logic_error("Cannot Open File " + runtime_filepath);
    }
    //  #  duration  waiting_num  route_num  connection_num
    runtime_file << "Cycle" << "\t";
    runtime_file << time_interval << "\t";
    runtime_file << net_manager->get_waiting_num() << "\t";
    runtime_file << cycle_finish_route << "\t";
    runtime_file << cycle_finish_cxn << endl;
    runtime_file << endl;
    runtime_file.close();
    return true;
}

bool QNetwork::finish() {
    current_time_point = get_current_time();
    cout << "Finished after " << get_time_second(current_time_point, start_time_point) << "s" << endl;
    ofstream metric_file;
    metric_file.open(metric_filepath, ios::app);
    if (!metric_file.is_open()) {
        cout << "Cannot Open File " << metric_filepath << endl;
        return false;
    }
    metric_file << "-------- Request --------" << endl;
    metric_file << "Request Rate: " << (double)IPS * TIME_PROB * net_manager->get_sd_num() * SD_PROB << "/s" << endl;
    metric_file << endl;
    metric_file << "-------- Process --------" << endl;
    metric_file << "Work Cycle: " << WORK_CYCLE << endl;
    metric_file << "Route Strategy: " << ROUTE_STRTG << endl;
    metric_file << "Resource Manage: " << RSRC_MANAGE << endl;
    metric_file << endl;
    metric_file << "-------- Optimization --------" << endl;
    metric_file << "Selected Requests: " << NUM_SELECTED_REQUESTS << endl;
    metric_file << "Repeats: " << NUM_REPEATS << endl;

    metric_file << endl;
    metric_file << "------ Performance ------" << endl;
    metric_file << "Connection Rate: " << (double)finished_cxn_num / RUN_TIME << "/s" << endl;
    double total_delay = 0;
    for (auto it_delay:net_manager->get_req_delay()) {
        total_delay += it_delay.second;
    }
    double average_delay = total_delay/(double)net_manager->get_req_delay().size();
    metric_file << "Average Delay: " << average_delay/IPS << "s" << endl;
    metric_file << endl;
    metric_file.close();
    return true;
}
