//
// Created by TianyaoChu on 2023/5/11.
//

#ifndef QUANTUM_NETWORKS_ROUTE_MANAGER_H
#define QUANTUM_NETWORKS_ROUTE_MANAGER_H

#include "user_service.h"
#include "net_resource.h"
#include "link_manager.h"
#include "net_topology/net_topology.h"
#include "net_topology/path.h"
#include "qdevice/qdevice.h"
#include <map>
#include <queue>
#include <vector>
#include <utility>

class RouteProject {
private:
    int route_id;
    map<int, LinkProject*> link_projects;   // map between edge id and link proj on it
    Path* path;
    UserRequest* request;
    LinkManager* link_manager;
    bool success;
    UserConnection* user_cxn;
    static int next_id;
public:
    RouteProject(int route_id, int rsrc_num, Path* path, UserRequest* request);
    ~RouteProject();
    int get_route_id() const;
    map<int, LinkProject*> get_link_projs();
    Path* get_path();
    UserRequest* get_request();
    void add_links(int edge_id, vector<EntangleLink*> links);
    map<int, int> update_entangles(int time);
    map<int, int> purify_available_links(HsRsrcManager* net_rsrc=nullptr);
    map<int, int> swap_all_connected(HsRsrcManager* net_rsrc=nullptr);
    bool check_user_connection();
    EntangleConnection* generate_connection(HsRsrcManager* net_rsrc=nullptr);
    bool is_success() const;
    void set_success(EntangleConnection* etg_cxn);
    UserConnection* get_user_cxn() const;
    void print_route_proj() const;
    void print_links() const;
    static int get_next_id();
    static void add_next_id();
};

class RouteManager {
private:
    map<int, RouteProject*> route_projects; // map from route id to project
    map<int, LinkGenerator*> link_generators;   // link generator for each edge
    NetTopology* net_topo;
public:
    explicit RouteManager(NetTopology* net_topo);
    ~RouteManager();
    void print_routing_projects() const;
    void add_new_routing(RouteProject* new_route_proj);
    void refresh_routing_state(int time, HsRsrcManager* net_rsrc= nullptr);
    map<int, UserConnection*> check_success_routing(RsrcManager* net_rsrc);
    map<int, UserConnection*> static_check_success_routing(RsrcManager* net_rsrc);
};

#endif //QUANTUM_NETWORKS_ROUTE_MANAGER_H
