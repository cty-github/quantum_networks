//
// Created by TianyaoChu on 2023/4/13.
//

#ifndef QUANTUM_NETWORKS_NET_MANAGER_H
#define QUANTUM_NETWORKS_NET_MANAGER_H

#include "user_service.h"
#include "net_resource.h"
#include "link_manager.h"
#include "route_manager.h"
#include "net_topology/net_topology.h"
#include "net_topology/path.h"
#include "qdevice/qdevice.h"
#include <utility>
#include <vector>
#include <queue>
#include <map>

class NetManager {
private:
    NetTopology* net_topo;
    map<int, SDPair*> sd_pairs;
    map<int, vector<Path*>> candidate_paths;
    map<int, pair<UserRequest*, int>> waiting_requests;
    map<int, pair<UserRequest*, int>> processing_requests;
    map<int, pair<UserRequest*, int>> serving_requests;
    NetResource* net_rsrc;
    RouteManager* route_manager;
    map<int, vector<UserConnection*>> user_connections;
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
    void print_serving_requests();
    void print_routing_projects();
    void print_user_connections();
    vector<UserRequest*> random_request(double sd_prob=0.4, double req_rate=0.2);
    void add_new_requests(const vector<UserRequest*>& new_requests);
    void reserve_resource(RouteProject* route_proj);
    vector<RouteProject*> calculate_new_routings();
    void schedule_new_routings();
    void refresh_routing_state(int time);
    void check_success_routing();
    void finish_user_connection(int time);
};

#endif //QUANTUM_NETWORKS_NET_MANAGER_H
