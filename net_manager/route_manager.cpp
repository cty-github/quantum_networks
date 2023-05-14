//
// Created by TianyaoChu on 2023/5/11.
//

#include "route_manager.h"
#include "utils/rand.h"
#include <iostream>

int RouteProject::next_id = 0;

RouteProject::~RouteProject() = default;

RouteManager::~RouteManager() = default;

RouteProject::RouteProject(int route_id, int rsrc_num, Path* path, UserRequest* request):
route_id(route_id), path(path), request(request), success(false), user_cxn(nullptr) {
    vector<QNode*> nodes = path->get_nodes();
    vector<QEdge*> edges = path->get_edges();
    for (int i = 0; i < edges.size(); i++) {
        int node_id_a = nodes[i]->get_id();
        int node_id_b = nodes[i+1]->get_id();
        int edge_id = edges[i]->get_edge_id();
        auto* link_proj = new LinkProject(node_id_a, node_id_b, edge_id,
                                          route_id, rsrc_num, 1);
        link_projects[edge_id] = link_proj;
    }
    link_manager = new LinkManager(link_projects);
}

int RouteProject::get_route_id() const {
    return route_id;
}

map<int, LinkProject*> RouteProject::get_link_projs() {
    return link_projects;
}

Path* RouteProject::get_path() {
    return path;
}

UserRequest* RouteProject::get_request() {
    return request;
}

void RouteProject::add_links(int edge_id, vector<EntangleLink*> links) {
    link_manager->add_links(edge_id, links);
}

map<int, int> RouteProject::update_links(int time) {
    return link_manager->update_links(time);
}

void RouteProject::purify_available_links() {
    link_manager->purify_available_links();
}

void RouteProject::swap_all_connected() {
    link_manager->swap_all_connected(path);
}

bool RouteProject::check_user_connection() {
    return link_manager->check_user_connection(request->get_s_node_id(), request->get_d_node_id());
}

EntangleConnection* RouteProject::generate_connection() {
    return link_manager->generate_connection(request->get_s_node_id(), request->get_d_node_id());
}

bool RouteProject::is_success() const {
    return success;
}

void RouteProject::set_success(EntangleConnection* etg_cxn) {
    success = true;
    user_cxn = new UserConnection(etg_cxn, request->get_request_id());
}

UserConnection* RouteProject::get_user_cxn() const {
    return user_cxn;
}

int RouteProject::get_next_id() {
    return next_id;
}

void RouteProject::add_next_id() {
    next_id++;
}

RouteManager::RouteManager(NetTopology* net_topo) {
    for (auto it:net_topo->get_edges()) {
        QEdge* edge = it.second;
        link_generators[it.first] = new LinkGenerator(it.first,
                                                      edge->get_node_id_a(),
                                                      edge->get_node_id_b(),
                                                      edge->get_success_rate(),
                                                      edge->get_ptn_src());
    }
}

void RouteManager::add_new_routing(RouteProject* new_route_proj) {
    route_projects[new_route_proj->get_route_id()] = new_route_proj;
    for (auto it:new_route_proj->get_link_projs()) {
        int edge_id = it.first;
        LinkProject* link_proj = it.second;
        link_generators[edge_id]->add_rsrc_num(link_proj->get_rsrc_num());
        link_generators[edge_id]->add_req_route(link_proj->get_route_id(), link_proj->get_req_num());
    }
}

void RouteManager::refresh_routing_state(int time) {
    for (auto it_route_proj:route_projects) {
        int route_id = it_route_proj.first;
        RouteProject* route_proj = it_route_proj.second;
        map<int, int> edge_expire_num = route_proj->update_links(time);
        for (auto it_edge_expire:edge_expire_num) {
            int edge_id = it_edge_expire.first;
            int expire_num = it_edge_expire.second;
            link_generators[edge_id]->add_req_route(route_id, expire_num);
        }
    }
    for (auto it_link_gen:link_generators) {
        int edge_id = it_link_gen.first;
        LinkGenerator* link_generator = it_link_gen.second;
        vector<EntangleLink*> new_links = link_generator->generate_links(time);
        map<int, vector<EntangleLink*>> serve_route_links = link_generator->serve_requests(new_links);
        for (const auto& route_link:serve_route_links) {
            int route_id = route_link.first;
            vector<EntangleLink*> links = route_link.second;
            route_projects[route_id]->add_links(edge_id, links);
        }
    }
    for (auto it_route_proj:route_projects) {
        RouteProject* route_proj = it_route_proj.second;
        route_proj->purify_available_links();
        route_proj->swap_all_connected();
        if (route_proj->check_user_connection()) {
            EntangleConnection* etg_cxn = route_proj->generate_connection();
            if (etg_cxn) {
                route_proj->set_success(etg_cxn);
            } else {
                cout << "Connect Links Fail" << endl;
            }
        }
    }
}

map<int, vector<UserConnection*>> RouteManager::check_success_project() {
    map<int, vector<UserConnection*>> req_user_cxn; // map from request id to constructed connections
    vector<int> success_routings;
    for (auto it_route_proj:route_projects) {
        int route_id = it_route_proj.first;
        RouteProject* route_proj = it_route_proj.second;
        if (route_proj->is_success()) {
            req_user_cxn[route_proj->get_request()->get_request_id()].push_back(route_proj->get_user_cxn());
        }
        success_routings.push_back(route_id);
    }
    for (auto route_id:success_routings) {
        route_projects.erase(route_id);
    }
    return req_user_cxn;
}