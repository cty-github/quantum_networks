//
// Created by TianyaoChu on 2023/3/25.
//

#include "qnetwork.h"
#include "utils/rand.h"
#include <sstream>
#include <fstream>
#include <iostream>

QNetwork::QNetwork(int ptn_src_num, int bsm_num,
                   int user_num, int repeater_num,
                   double size, double alpha, double beta,
                   double decay_rate, double z_fidelity, double x_fidelity) {
    device_manager = new DeviceManager(ptn_src_num, bsm_num, size, decay_rate, z_fidelity, x_fidelity);
    net_topo = new NetTopology(device_manager, user_num, repeater_num, size, alpha, beta);
    net_manager = new NetManager(net_topo, user_num);
}

QNetwork::QNetwork(const string& net_dev_filepath, const string& net_topo_filepath, const string& sd_pair_filepath) {
    device_manager = new DeviceManager(net_dev_filepath);
    net_topo = new NetTopology(net_topo_filepath, device_manager);
    net_manager = new NetManager(sd_pair_filepath, net_topo);
}

QNetwork::~QNetwork() = default;

int QNetwork::get_node_num() const {
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return 0;
    }
    return net_topo->get_node_num();
}

int QNetwork::get_edge_num() const {
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return 0;
    }
    return net_topo->get_edge_num();
}

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
    cout << "PtnSrc request_num: " << device_manager->get_ptn_src_num() << endl;
    cout << "BSM request_num: " << device_manager->get_bsm_num() << endl;
    cout << "Node request_num: " << net_topo->get_node_num() << endl;
    cout << "Edge request_num: " << net_topo->get_edge_num() << endl;
    cout << endl;
    return net_manager->initialize(k);
}

bool QNetwork::work_cycle() {
    cout << "Work Cycle" << endl;
    cout << "----------" << endl;
    net_manager->add_new_requests(net_manager->random_request(0.4, 0.2));
    net_manager->print_waiting_requests();
    net_manager->schedule_new_routings();
    net_manager->print_processing_requests();

    net_manager->refresh_routing_state();
    net_manager->check_success_project();
    net_manager->end_user_connection();
    return true;
}

//EntangleConnection* QNetwork::generate_etg_route(Path* route) {
//    if (!link_mgr) {
//        cout << "No Link Manager" << endl;
//        return nullptr;
//    }
//    return link_mgr->connect_links(link_mgr->generate_links(route), route);
//}
