//
// Created by TianyaoChu on 2023/4/13.
//

#ifndef QUANTUM_NETWORKS_NET_MANAGER_H
#define QUANTUM_NETWORKS_NET_MANAGER_H

#include "user_service.h"
#include "net_resource.h"
#include "link_manager.h"
#include "net_topology/net_topology.h"
#include "net_topology/path.h"
#include "qdevice/qdevice.h"
#include <utility>
#include <vector>
#include <queue>
#include <map>

class RouteProject {
private:
    vector<LinkProject*> link_projects;
    Path* path;
    UserRequest* request;
    bool success;
public:
    RouteProject(int rsrc_num, Path* path, UserRequest* request);
    ~RouteProject();
    vector<LinkProject*> get_link_projs();
    Path* get_path();
    UserRequest* get_request();
    bool is_success() const;
    void set_success();
};

class RouteManager {
private:
    vector<RouteProject*> route_projects;
    LinkManager* link_manager;
public:
    RouteManager();
    ~RouteManager();
    void add_new_routing(RouteProject* new_route_proj);
    void refresh_routing_state();
};

class NetManager {
private:
    NetTopology* net_topo;
    map<int, SDPair*> sd_pairs;
    map<int, vector<Path*>> candidate_paths;
    map<int, pair<UserRequest*, int>> waiting_requests;
    map<int, pair<UserRequest*, int>> processing_requests;
    NetResource* net_rsrc;
    RouteManager* route_manager;
    vector<UserConnection*> user_connections;
public:
    NetManager(NetTopology* net_topo, int user_num);
    NetManager(const string& filepath, NetTopology* net_topo);
    ~NetManager();
    void load_sd_pairs(const string& filepath);
    bool save_sd_pairs(const string& filepath) const;
    Path* get_path(int src_node_id, int dst_node_id);
    vector<Path*> get_paths(int src_node_id, int dst_node_id, int k);
    bool initialize(int k);
    void print_waiting_requests();
    void print_processing_requests();
    vector<UserRequest*> random_request(double sd_prob=0.4, double req_rate=0.2);
    void add_new_requests(const vector<UserRequest*>& new_requests);
    vector<RouteProject*> calculate_new_routings();
    void schedule_new_routings();
    void refresh_routing_state();
    void check_success_project();
    void end_user_connection();
};

#endif //QUANTUM_NETWORKS_NET_MANAGER_H
