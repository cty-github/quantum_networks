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
    map<int, UserRequest*> user_requests;   // one connection for one request
    map<int, UserRequest*> waiting_requests;
    map<int, UserRequest*> processing_requests;
    map<int, UserRequest*> serving_requests;
    RsrcManager* net_rsrc;
//    union{BasicRsrcManager* basic; HsRsrcManager* hs;} net_rsrc;
    RouteManager* route_manager;
    map<int, UserConnection*> user_connections;
public:
    NetManager(NetTopology* net_topo, int user_num);
    NetManager(const string& filepath, NetTopology* net_topo);
    ~NetManager();
    int get_sd_num() const;
    void load_sd_pairs(const string& filepath);
    bool save_sd_pairs(const string& filepath) const;
    Path* get_path(int src_node_id, int dst_node_id);
    vector<Path*> get_paths(int src_node_id, int dst_node_id, int k);
    bool initialize(int k);
    int get_waiting_num() const;
    void print_waiting_requests() const;
    int get_processing_num() const;
    void print_processing_requests() const;
    int get_serving_num() const;
    void print_serving_requests() const;
    void print_routing_projects() const;
    int get_user_cxn_num() const;
    void print_user_connections() const;
    vector<UserRequest*> random_request(int time, double time_prob, double sd_prob, double req_rate);
    void add_new_requests(const vector<UserRequest*>& new_requests);
    void reserve_resource(RouteProject* route_proj);
    vector<RouteProject*> greedy_routing_projects(RsrcManager* tmp_rsrc) const;
    vector<RouteProject*> heuristic_routing_projects(RsrcManager* tmp_rsrc) const;
    vector<RouteProject*> calculate_new_routings(RsrcManager* tmp_rsrc) const;
    void schedule_new_routings();
    void refresh_routing_state(int time);
    int check_success_routing(const string& runtime_filepath);
    int finish_user_connection(int time);
    static double calculate_obj(UserRequest* request, Path* path);
};

#endif //QUANTUM_NETWORKS_NET_MANAGER_H
