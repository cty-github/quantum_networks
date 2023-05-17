//
// Created by TianyaoChu on 2023/4/13.
//

#include "net_manager.h"
#include "test_params.h"
#include "utils/rand.h"
#include <fstream>
#include <iostream>
#include <iomanip>

NetManager::NetManager(NetTopology* net_topo, int user_num): net_topo(net_topo) {
    double request_rate = SD_RATIO/user_num;
    uniform_real_distribution<double> rand_double(0.0,1.0);
    uniform_real_distribution<double> rand_fidelity(SD_FIDE_LOW,SD_FIDE_UP);
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
    route_manager = new RouteManager(net_topo);
}

NetManager::NetManager(const string& filepath, NetTopology *net_topo): net_topo(net_topo) {
    load_sd_pairs(filepath);
    net_rsrc = new NetResource(net_topo);
    route_manager = new RouteManager(net_topo);
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
    file << setprecision(6);
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
//        cout << "Shortest Route from " << sd_pair->get_s_node_id()
//             << " to " << sd_pair->get_d_node_id() << endl;
//        for (auto path:candidate_paths[it.first]) {
//            path->print_path();
//            cout << endl;
//        }
//        cout << endl;
    }
    return true;
}

int NetManager::get_waiting_request_num() const {
    int waiting_num = 0;
    for(auto request:waiting_requests) {
        waiting_num += request.second.second;
    }
    return waiting_num;
}

void NetManager::print_waiting_requests() {
    cout << "- Waiting Requests" << endl;
    int waiting_num = 0;
    for(auto request:waiting_requests) {
//        request.second.first->print_request();
//        cout << " Num " << request.second.second << endl;
        waiting_num += request.second.second;
    }
    cout << "Waiting Num: " << waiting_num << endl;
}

void NetManager::print_processing_requests() {
    cout << "- Processing Requests" << endl;
    int processing_num = 0;
    for(auto request:processing_requests) {
//        request.second.first->print_request();
//        cout << " Num " << request.second.second << endl;
        processing_num += request.second.second;
    }
    cout << "Processing Num: " << processing_num << endl;
}

void NetManager::print_serving_requests() {
    cout << "- Serving Requests" << endl;
    int serving_num = 0;
    for(auto request:serving_requests) {
//        request.second.first->print_request();
//        cout << " Num " << request.second.second << endl;
        serving_num += request.second.second;
    }
    cout << "Serving Num: " << serving_num << endl;
}

void NetManager::print_routing_projects() {
    route_manager->print_routing_projects();
}

void NetManager::print_user_connections() {
    cout << "- User Connections" << endl;
    int cxn_num = 0;
    for(const auto& it_cxn:user_connections) {
//        int request_id = it_cxn.first;
//        for (auto cxn:it_cxn.second) {
//            cout << "Connection " << cxn->get_connection_id() << " Serve Request " << request_id << endl;
//        }
        cxn_num += (int)it_cxn.second.size();
    }
    cout << "User Connection Num: " << cxn_num << endl;
}

vector<UserRequest*> NetManager::random_request(int time, double time_prob, double sd_prob, double req_rate) {
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    uniform_real_distribution<double> rand_double(0.0,1.0);
    vector<UserRequest*> random_requests;
    for (int i = 0; i < time; i++) {
        if (rand_double(rand_eng) > time_prob) {
            continue;
        }
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
    }
    return random_requests;
}

void NetManager::add_new_requests(const vector<UserRequest*>& new_requests) {
    for (auto request:new_requests) {
        user_requests[request->get_request_id()] = request;
        waiting_requests[request->get_request_id()] = {request, request->get_request_num()};
    }
}

void NetManager::reserve_resource(RouteProject* route_proj) {
//    cout << "Reserve Resource of Route " << route_proj->get_route_id() << endl;
    for (auto it:route_proj->get_link_projs()) {
        LinkProject* link_proj = it.second;
        net_rsrc->reserve_link_resource(link_proj->get_s_node_id(),
                                        link_proj->get_d_node_id(),
                                        link_proj->get_rsrc_num());
    }
}

vector<RouteProject*> NetManager::calculate_new_routings() {
    // greedy realization for test
    vector<RouteProject*> new_route_projects;
    for (auto it_wait:waiting_requests) {
        UserRequest* serve_request = it_wait.second.first;
        int req_num = it_wait.second.second;
        int pair_id = serve_request->get_pair_id();
        for (auto path:candidate_paths[pair_id]) {
            vector<QEdge*> edges = path->get_edges();
            int rsrc_num;
            for (int i = 0; i < edges.size(); i++) {
                int node_id_a = edges[i]->get_node_id_a();
                int node_id_b = edges[i]->get_node_id_b();
                int max_num = net_rsrc->max_link_resource(node_id_a, node_id_b);
                if (i == 0) {
                    rsrc_num = max_num;
                }
                if (max_num < rsrc_num) {
                    rsrc_num = max_num;
                }
            }
            if (rsrc_num == 0) {
                continue;
            } else if (rsrc_num < req_num) {
                for (int i = 0; i < rsrc_num; i++) {
                    auto* route_proj = new RouteProject(RouteProject::get_next_id(), 1,
                                                        path, serve_request);
                    RouteProject::add_next_id();
                    new_route_projects.push_back(route_proj);
                    reserve_resource(route_proj);
                }
                req_num -= rsrc_num;
            } else {
                for (int i = 0; i < req_num; i++) {
                    RouteProject* route_proj;
                    if (i == 0) {
                        route_proj = new RouteProject(RouteProject::get_next_id(),
                                                      rsrc_num - req_num + 1,
                                                      path, serve_request);
                    } else {
                        route_proj = new RouteProject(RouteProject::get_next_id(), 1,
                                                      path, serve_request);
                    }
                    RouteProject::add_next_id();
                    new_route_projects.push_back(route_proj);
                    reserve_resource(route_proj);
                }
                break;
            }
        }
    }
    cout << "New Routing Num: " << new_route_projects.size() << endl;
    return new_route_projects;
}

void NetManager::schedule_new_routings() {
    cout << "- New Routings" << endl;
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
    }
}

void NetManager::refresh_routing_state(int time) {
    if (route_manager == nullptr) {
        cout << "No Route Manager" << endl;
        return;
    }
    route_manager->refresh_routing_state(time);
}

bool NetManager::check_success_routing(const string& output_filepath) {
    cout << "- Finish Routings" << endl;
    ofstream file;
    file.open(output_filepath,ios::app);
    if (!file.is_open()) {
        cout << "Cannot Open File " << output_filepath << endl;
        return false;
    }
    file << setiosflags(ios::fixed) << setprecision(6);
    int success_route_num = 0;
    for (auto it_new_cxn: route_manager->check_success_routing(net_rsrc)) {
        int request_id = it_new_cxn.first;
        vector<UserConnection*>& new_cxn = it_new_cxn.second;
        for (auto cxn:new_cxn) {
            double task_completion_time = get_time_second(cxn->get_created_time(),
                                                          user_requests[request_id]->get_start_time());
            file << "TaskCpl" << "\t";
            file << request_id << "\t\t";
            file << user_requests[request_id]->get_pair_id() << "\t\t";
            file << cxn->get_connection_id() << "\t\t";
            file << user_requests[request_id]->get_fide_th() << "\t";
            file << cxn->get_fidelity() << "\t";
            file << task_completion_time << endl;
        }
        user_connections[request_id].insert(user_connections[request_id].end(),
                                            new_cxn.begin(), new_cxn.end());
        UserRequest* request = user_requests[request_id];
        int processing_num = processing_requests[request_id].second;
        int success_num = (int)new_cxn.size();
        success_route_num += success_num;
        if (processing_num == success_num) {
            processing_requests.erase(request_id);
        } else {
            processing_requests[request_id].second -= success_num;
        }
        user_requests[request_id]->add_processed_num(success_num);
        if (serving_requests.find(request_id) == serving_requests.end()) {
            serving_requests[request_id] = {request, success_num};
        } else {
            serving_requests[request_id].second += success_num;
        }
    }
    file.close();
    cout << "Finish Routing Num: " << success_route_num << endl;
    return true;
}

int NetManager::finish_user_connection(int time) {
    cout << "- Finish Connections" << endl;
    vector<int> all_expire_request;
    int finish_cxn_num = 0;
    for (auto& it_user_cxn:user_connections) {
        int request_id = it_user_cxn.first;
        vector<UserConnection*>& user_cxns = it_user_cxn.second;
        vector<int> expire_cxns;
        for (int i = 0; i < user_cxns.size(); i++) {
            user_cxns[i]->add_age(time);
            if (user_cxns[i]->is_expired()) {
                expire_cxns.push_back(i);
            }
        }
        for (int i = (int)expire_cxns.size() - 1; i >= 0; i--) {
            user_cxns[expire_cxns[i]]->finish_connection();
            delete user_cxns[expire_cxns[i]];
            user_cxns.erase(user_cxns.begin() + expire_cxns[i]);
        }
        int expire_num = (int)expire_cxns.size();
        finish_cxn_num += expire_num;
        int s_id = serving_requests[request_id].first->get_s_node_id();
        int d_id = serving_requests[request_id].first->get_d_node_id();
        net_rsrc->release_node_memory(s_id, expire_num);
        net_rsrc->release_node_memory(d_id, expire_num);
        serving_requests[request_id].second -= expire_num;
        if (serving_requests[request_id].second == 0) {
            serving_requests.erase(request_id);
            all_expire_request.push_back(request_id);
        }
        user_requests[request_id]->add_served_num(expire_num);
        if (user_requests[request_id]->has_end()) {
            delete user_requests[request_id];
            user_requests.erase(request_id);
        }
    }
    for (auto request_id:all_expire_request) {
        user_connections.erase(request_id);
    }
    cout << "Finish Connection Num: " << finish_cxn_num << endl;
    return finish_cxn_num;
}
