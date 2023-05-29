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

map<int, int> RouteProject::update_entangles(int time) {
    return link_manager->update_entangles(time);
}

void RouteProject::purify_available_links() {
    link_manager->purify_available_links(path);
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
    user_cxn = new UserConnection(etg_cxn, UserConnection::get_next_id(),
                                  request->get_request_id());
    UserConnection::add_next_id();
}

UserConnection* RouteProject::get_user_cxn() const {
    return user_cxn;
}

void RouteProject::print_route_proj() const {
    cout << "Route " << route_id << ": Request " << request->get_request_id() << " ";
    path->print_path();
    cout << endl;
}

void RouteProject::print_links() const {
    cout << "Route " << route_id << ": ";
    link_manager->print_links();
    if (user_cxn) {
        cout << "User Connection " << user_cxn->get_connection_id() << " ";
        cout << "(" << user_cxn->get_s_node_id() << "->" << user_cxn->get_d_node_id() << ") ";
    }
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

void RouteManager::print_routing_projects() const {
    cout << "- Routing Projects" << endl;
//    for (auto it:route_projects) {
//        it.second->print_route_proj();
//    }
    cout << "Routing Projects Num: " << route_projects.size() << endl;
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
    //  - Update Old Links
    for (auto it_route_proj:route_projects) {
        int route_id = it_route_proj.first;
        RouteProject* route_proj = it_route_proj.second;
        map<int, int> edge_expire_num = route_proj->update_entangles(time);
        for (auto it_edge_expire:edge_expire_num) {
            int edge_id = it_edge_expire.first;
            int expire_num = it_edge_expire.second;
            link_generators[edge_id]->add_req_route(route_id, expire_num);
        }
//        route_proj->print_links();
//        cout << endl;
    }
    //  - Generate New Links
    int new_link_num = 0;
    for (auto it_link_gen:link_generators) {
        int edge_id = it_link_gen.first;
        LinkGenerator* link_generator = it_link_gen.second;
        if (link_generator->get_total_rsrc_num()>0 && link_generator->get_total_req_num()>0) {
//            cout << "Edge " << edge_id << ": ";
            vector<EntangleLink*> new_links = link_generator->generate_links(time);
//            cout << new_links.size();
            new_link_num += (int)new_links.size();
            map<int, vector<EntangleLink*>> serve_route_links = link_generator->serve_requests(new_links);
            for (const auto& route_link:serve_route_links) {
                int route_id = route_link.first;
                vector<EntangleLink*> links = route_link.second;
                route_projects[route_id]->add_links(edge_id, links);
//                cout << " " << route_id << "-" << links.size();
            }
//            cout << endl;
        }
    }
//    cout << "New Link Num: " << new_link_num << endl;
    //  - Purify and Swap Links
    for (auto it_route_proj:route_projects) {
        RouteProject* route_proj = it_route_proj.second;
        route_proj->purify_available_links();
        route_proj->swap_all_connected();
        if (route_proj->check_user_connection()) {
            EntangleConnection* etg_cxn = route_proj->generate_connection();
            route_proj->set_success(etg_cxn);
        }
//        route_proj->print_links();
        if (route_proj->is_success()) {
//            cout << "Routing Success";
        }
//        cout << endl;
    }
}

map<int, vector<UserConnection*>> RouteManager::check_success_routing(NetResource* net_rsrc) {
    map<int, vector<UserConnection*>> req_user_cxn; // map from request id to constructed connections
    vector<int> success_routings;
    for (auto it_route_proj:route_projects) {
        int route_id = it_route_proj.first;
        RouteProject* route_proj = it_route_proj.second;
        if (route_proj->is_success()) {
            req_user_cxn[route_proj->get_request()->get_request_id()].push_back(route_proj->get_user_cxn());
            success_routings.push_back(route_id);
//            cout << "Release Resource of Route " << route_id << endl;
            for (auto it:route_proj->get_link_projs()) {
                int edge_id = it.first;
                LinkProject* link_proj = it.second;
                link_generators[edge_id]->sub_rsrc_num(link_proj->get_rsrc_num());
                net_rsrc->release_link_resource(link_proj->get_s_node_id(),
                                                link_proj->get_d_node_id(),
                                                link_proj->get_rsrc_num());
            }
            net_rsrc->reserve_node_memory(route_proj->get_user_cxn()->get_s_node_id(), 1);
            net_rsrc->reserve_node_memory(route_proj->get_user_cxn()->get_d_node_id(), 1);
        }
    }
    for (auto route_id:success_routings) {
        route_projects.erase(route_id);
    }
    return req_user_cxn;
}
