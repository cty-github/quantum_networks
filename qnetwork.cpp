//
// Created by TianyaoChu on 2023/3/25.
//

#include "qnetwork.h"
#include "utils/rand.h"
#include <sstream>
#include <fstream>
#include <iostream>

QNetwork::QNetwork() = default;

QNetwork::QNetwork(int ptn_src_num, int bsm_num,
                   int user_num, int repeater_num,
                   double size, double alpha, double beta,
                   double decay_rate, double z_fidelity, double x_fidelity) {
    device_manager = new DeviceManager(ptn_src_num, bsm_num, size, decay_rate, z_fidelity, x_fidelity);
    net_topology = new NetTopology(device_manager, user_num, repeater_num, size, alpha, beta);
    double request_rate = 1.0/user_num;
    uniform_real_distribution<double> rand_double(0.0,1.0);
    uniform_real_distribution<double> rand_fidelity(0.95,0.99);
    for (int i = 0; i < user_num; i++) {
        for (int j = i+1; j < user_num; j++) {
            if (rand_double(rand_eng) < request_rate) {
                request_pairs.emplace_back(i, j, rand_fidelity(rand_eng));
            }
            if (rand_double(rand_eng) < request_rate) {
                request_pairs.emplace_back(j, i, rand_fidelity(rand_eng));
            }
        }
    }
    link_mgr = new LinkManager(net_topology);
}

QNetwork::QNetwork(const string& net_dev_filepath, const string& net_topo_filepath, const string& request_filepath) {
    device_manager = new DeviceManager(net_dev_filepath);
    net_topology = new NetTopology(net_topo_filepath, device_manager);
    load_req(request_filepath);
}

QNetwork::~QNetwork() = default;

int QNetwork::get_node_num() const {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return 0;
    }
    return net_topology->get_node_num();
}

int QNetwork::get_edge_num() const {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return 0;
    }
    return net_topology->get_edge_num();
}

bool QNetwork::draw_net_topo(const string& filepath) const {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return false;
    }
    return false;
}

void QNetwork::load_req(const string& filepath) {
    ifstream file;
    file.open(filepath,ios::in);
    if (!file.is_open()) {
        cout << "Cannot Open File " << filepath << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        stringstream ss(line);
        string key;
        ss >> key;
        if (key == "request") {
            int s_id;
            int d_id;
            double fidelity;
            ss >> s_id >> d_id >> fidelity;
            request_pairs.emplace_back(s_id, d_id, fidelity);
        } else {
            cout << "Error in File" << endl;
        }
    }
    file.close();
}

bool QNetwork::save_req(const string& filepath) const {
    ofstream file;
    file.open(filepath,ios::out);
    if (!file.is_open()) {
        cout << "Cannot Open File " << filepath << endl;
        return false;
    }
    file << "#\ts_id\td_id\tfide_th" << endl;
    for (auto & it : request_pairs) {
        file << "request\t";
        file << std::get<0>(it) << '\t';
        file << std::get<1>(it) << '\t';
        file << std::get<2>(it) << endl;
    }
    file << endl;
    file.close();
    return true;
}

bool QNetwork::save_net(const string& net_dev_filepath,
                        const string& net_topo_filepath,
                        const string& request_filepath) const {
    if (device_manager == nullptr) {
        cout << "No Device Manager" << endl;
        return false;
    }
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return false;
    }
    if (!device_manager->save_dev(net_dev_filepath)) {
        cout << "Save Device Fail" << endl;
        return false;
    }
    if (!net_topology->save_topo(net_topo_filepath)) {
        cout << "Save Topo Fail" << endl;
        return false;
    }
    if (!save_req(request_filepath)) {
        cout << "Save Request Fail" << endl;
        return false;
    }
    return true;
}

Path* QNetwork::get_path(int src_node_id, int dst_node_id) {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    return net_topology->get_path(src_node_id, dst_node_id);
}

vector<Path*> QNetwork::get_paths(int src_node_id, int dst_node_id, int k) {
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    return net_topology->get_paths(src_node_id, dst_node_id, k);
}

bool QNetwork::initialize(int k) {
    if (device_manager == nullptr) {
        cout << "No Device Manager" << endl;
        return false;
    }
    if (net_topology == nullptr) {
        cout << "No Net Topology" << endl;
        return false;
    }
    cout << "PtnSrc num: " << device_manager->get_ptn_src_num() << endl;
    cout << "BSM num: " << device_manager->get_bsm_num() << endl;
    cout << "Node num: " << net_topology->get_node_num() << endl;
    cout << "Edge num: " << net_topology->get_edge_num() << endl;
    cout << endl;

    // prepare k shortest paths as candidate paths for each request pair
    for (auto request_pair:request_pairs) {
        candidate_paths.push_back(get_paths(std::get<0>(request_pair),
                                            std::get<1>(request_pair), k));
        cout << "Shortest Route from " << std::get<0>(request_pair)
                << " to " << std::get<1>(request_pair) << endl;
        for (auto path:candidate_paths.back()) {
            path->print_path();
        }
        cout << endl;
    }
    return true;
}

EntangleRoute* QNetwork::generate_etg_route(Path* route) {
    if (!link_mgr) {
        cout << "No Link Manager" << endl;
        return nullptr;
    }
    return link_mgr->connect_links(link_mgr->generate_links(route), route);
}

void QNetwork::run_cycle() {}
