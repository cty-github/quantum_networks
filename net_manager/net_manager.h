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
    vector<LinkProject*> link_project;
    LinkManager* link_manager{};
    bool success;
public:
    RouteProject();
    ~RouteProject();
};

class RouteManager {
private:
    vector<RouteProject*> route_projects;
    LinkManager* recover_links{};
public:
    RouteManager();
    ~RouteManager();
};

class NetManager {
private:
    queue<UserRequest*> waiting_requests;
    vector<UserRequest*> processing_requests;
    NetResource* net_rsrc{};
    RouteManager* route_manager{};
    vector<UserConnection*> user_connections;
public:
    explicit NetManager(NetTopology* net_topo);
    ~NetManager();
    void print_waiting_requests();
    void add_new_requests(const vector<UserRequest*>& new_requests);
    void schedule_new_routing();
    void refresh_routing_state();
    void check_success_project();
    void end_user_connection();
};

#endif //QUANTUM_NETWORKS_NET_MANAGER_H
