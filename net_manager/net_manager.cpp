//
// Created by TianyaoChu on 2023/4/13.
//

#include "net_manager.h"
#include "utils/rand.h"
#include <sstream>
#include <fstream>
#include <iostream>

RouteProject::RouteProject(int rsrc_num, Path* path, UserRequest* request):
path(path), request(request), success(false) {
    vector<QNode*> nodes = path->get_nodes();
    vector<QEdge*> edges = path->get_edges();
    for (int i = 0; i < edges.size(); i++) {
        int node_id_a = nodes[i]->get_id();
        int node_id_b = nodes[i+1]->get_id();
        int edge_id = edges[i]->get_edge_id();
        auto* link_prj = new LinkProject(node_id_a, node_id_b, edge_id,
                                         rsrc_num, request);
        link_projects.push_back(link_prj);
    }
}

RouteProject::~RouteProject() = default;

vector<LinkProject*> RouteProject::get_link_projs() {
    return link_projects;
}

Path* RouteProject::get_path() {
    return path;
}

UserRequest* RouteProject::get_request() {
    return request;
}

bool RouteProject::is_success() const {
    return success;
}

void RouteProject::set_success() {
    success = true;
}

RouteManager::RouteManager(): link_manager(nullptr) {}

RouteManager::~RouteManager() = default;

void RouteManager::add_new_routing(RouteProject* new_route_proj) {
    route_projects.push_back(new_route_proj);
}

void RouteManager::refresh_routing_state() {

}

NetManager::NetManager(NetTopology* net_topo, int user_num): net_topo(net_topo) {
    double request_rate = 1.0/user_num;
    uniform_real_distribution<double> rand_double(0.0,1.0);
    uniform_real_distribution<double> rand_fidelity(0.95,0.99);
    for (int i = 0; i < user_num; i++) {
        for (int j = 0; j < user_num; j++) {
            if (i != j && rand_double(rand_eng) < request_rate) {
                sd_pairs[SDPair::get_next_id()] = new SDPair(SDPair::get_next_id(),
                                                             i, j,
                                                             rand_fidelity(rand_eng));
                SDPair::add_next_id();
            }
        }
    }
    net_rsrc = new NetResource(net_topo);
    route_manager = new RouteManager();
}

NetManager::NetManager(const string& filepath, NetTopology *net_topo): net_topo(net_topo) {
    load_sd_pairs(filepath);
    net_rsrc = new NetResource(net_topo);
    route_manager = new RouteManager();
}

NetManager::~NetManager() = default;

void NetManager::load_sd_pairs(const string& filepath) {
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
        if (key == "sd_pair") {
            int pair_id;
            int s_id;
            int d_id;
            double fidelity;
            ss >> pair_id >> s_id >> d_id >> fidelity;
            sd_pairs[pair_id] = new SDPair(pair_id, s_id, d_id, fidelity);
        } else {
            cout << "Error in File" << endl;
        }
    }
    file.close();
}

bool NetManager::save_sd_pairs(const string& filepath) const {
    ofstream file;
    file.open(filepath,ios::out);
    if (!file.is_open()) {
        cout << "Cannot Open File " << filepath << endl;
        return false;
    }
    file << "#\t\tpair_id\ts_id\td_id\tfide_th" << endl;
    for (auto & it:sd_pairs) {
        SDPair* sd_pair = it.second;
        file << "sd_pair\t";
        file << sd_pair->get_pair_id() << "\t\t";
        file << sd_pair->get_s_node_id() << "\t\t";
        file << sd_pair->get_d_node_id() << "\t\t";
        file << sd_pair->get_fide_th() << endl;
    }
    file << endl;
    file.close();
    return true;
}

Path* NetManager::get_path(int src_node_id, int dst_node_id) {
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    return net_topo->get_path(src_node_id, dst_node_id);
}

vector<Path*> NetManager::get_paths(int src_node_id, int dst_node_id, int k) {
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    return net_topo->get_paths(src_node_id, dst_node_id, k);
}

bool NetManager::initialize(int k) {
    // prepare k shortest paths as candidate paths for each request pair
    for (auto it:sd_pairs) {
        SDPair* sd_pair = it.second;
        candidate_paths[it.first]=get_paths(sd_pair->get_s_node_id(),
                                            sd_pair->get_d_node_id(), k);
        cout << "Shortest Route from " << sd_pair->get_s_node_id()
             << " to " << sd_pair->get_d_node_id() << endl;
        for (auto path:candidate_paths[it.first]) {
            path->print_path();
        }
        cout << endl;
    }
    return true;
}

void NetManager::print_waiting_requests() {
    cout << "Waiting Requests: " << endl;
    for(auto request:waiting_requests) {
        request.second.first->print_request();
        cout << " num: " << request.second.second << endl;
    }
    cout << endl;
}

void NetManager::print_processing_requests() {
    cout << "Processing Requests: " << endl;
    for(auto request:processing_requests) {
        request.second.first->print_request();
        cout << " num: " << request.second.second << endl;
    }
    cout << endl;
}

vector<UserRequest*> NetManager::random_request(double sd_prob, double req_rate) {
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    uniform_real_distribution<double> rand_double(0.0,1.0);
    vector<UserRequest*> random_requests;
    for (auto it:sd_pairs) {
        SDPair* sd_pair = it.second;
        if (rand_double(rand_eng) < sd_prob) {
            int s_id = sd_pair->get_s_node_id();
            int d_id = sd_pair->get_d_node_id();
            double fide_th = sd_pair->get_fide_th();
            int max_request = (int)(std::sqrt(net_topo->get_node(s_id)->get_memory_size() *
                                              net_topo->get_node(d_id)->get_memory_size()) * req_rate);
            uniform_int_distribution<int> rand_int(0,max_request);
            int num_request = rand_int(rand_eng);
            if (num_request == 0) {
                continue;
            }
            auto* user_request = new UserRequest(UserRequest::get_next_id(), it.first,
                                                 s_id, d_id,
                                                 fide_th, num_request);
            UserRequest::add_next_id();
            random_requests.push_back(user_request);
        }
    }
    return random_requests;
}

void NetManager::add_new_requests(const vector<UserRequest*>& new_requests) {
    for (auto request:new_requests) {
        waiting_requests[request->get_request_id()] = {request, request->get_request_num()};
    }
}

vector<RouteProject*> NetManager::calculate_new_routings() {
    vector<RouteProject*> new_route_projects;
    UserRequest* serve_request = waiting_requests.begin()->second.first;
    int pair_id = serve_request->get_pair_id();
    Path* shortest_path = candidate_paths[pair_id][0];
    vector<QEdge*> edges = shortest_path->get_edges();
    int rsrc_num;
    for (int i = 0; i < edges.size(); i++) {
        int node_id_a = edges[i]->get_node_id_a();
        int node_id_b = edges[i]->get_node_id_b();
        int max_num = net_rsrc->max_resource(node_id_a, node_id_b);
        if (i == 0) {
            rsrc_num = max_num;
        }
        if (max_num < rsrc_num) {
            rsrc_num = max_num;
        }
    }
    auto* route_prj = new RouteProject(rsrc_num, shortest_path, serve_request);
    new_route_projects.push_back(route_prj);
    return new_route_projects;
}

void NetManager::schedule_new_routings() {
    vector<RouteProject*> new_route_projects = calculate_new_routings();
    for (auto new_route_proj:new_route_projects) {
        UserRequest* request = new_route_proj->get_request();
        int request_id = request->get_request_id();
        int waiting_num = waiting_requests[request_id].second;
        if (waiting_num == 1) {
            waiting_requests.erase(request_id);
        } else {
            waiting_requests[request_id].second--;
        }
        if (processing_requests.find(request_id) == processing_requests.end()) {
            processing_requests[request_id] = {request, 1};
        } else {
            processing_requests[request_id].second++;
        }
        route_manager->add_new_routing(new_route_proj);
        for (auto link_proj:new_route_proj->get_link_projs()) {
            net_rsrc->reserve_resource(link_proj->get_s_node_id(),
                                       link_proj->get_d_node_id(),
                                       link_proj->get_rsrc_num());
        }
    }
}

void NetManager::refresh_routing_state() {
    if (route_manager == nullptr) {
        cout << "No Route Manager" << endl;
        return;
    }
    route_manager->refresh_routing_state();
}

void NetManager::check_success_project() {

}

void NetManager::end_user_connection() {

}
