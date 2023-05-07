//
// Created by TianyaoChu on 2023/4/13.
//

#include "net_manager.h"

RouteProject::RouteProject() {}

RouteProject::~RouteProject() = default;

RouteManager::RouteManager() {}

RouteManager::~RouteManager() = default;

NetManager::NetManager(NetTopology* net_topo) {
    net_rsrc = new NetResource(net_topo);
}

NetManager::~NetManager() = default;

void NetManager::print_waiting_requests() {
    for(int i = 0; i < waiting_requests.size(); i++) {
        UserRequest* request = waiting_requests.front();
        request->print_request();
        waiting_requests.push(request);
        waiting_requests.pop();
    }
}

void NetManager::add_new_requests(const vector<UserRequest*>& new_requests) {
    for (auto request:new_requests) {
        waiting_requests.push(request);
    }
}

void NetManager::schedule_new_routing() {

}

void NetManager::refresh_routing_state() {

}

void NetManager::check_success_project() {

}

void NetManager::end_user_connection() {

}
