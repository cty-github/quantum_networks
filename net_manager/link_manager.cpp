//
// Created by TianyaoChu on 2023/5/7.
//

#include "link_manager.h"
#include "utils/rand.h"
#include "test_params.h"
#include <iostream>
#include <utility>

LinkProject::LinkProject(int s_node_id, int d_node_id, int edge_id, int route_id, int total_rsrc_num, int req_num,
                         pair<int, int> src_rsrc, pair<int, int> dst_rsrc, pair<int, int> edge_rsrc):
s_node_id(s_node_id), d_node_id(d_node_id), edge_id(edge_id), route_id(route_id),
total_rsrc_num(total_rsrc_num), req_num(req_num),
src_rsrc(std::move(src_rsrc)), dst_rsrc(std::move(dst_rsrc)), edge_rsrc(std::move(edge_rsrc)) {}

LinkProject::~LinkProject() = default;

int LinkProject::get_s_node_id() const {
    return s_node_id;
}

int LinkProject::get_d_node_id() const {
    return d_node_id;
}

int LinkProject::get_edge_id() const {
    return edge_id;
}

int LinkProject::get_route_id() const {
    return route_id;
}

int LinkProject::get_total_rsrc_num() const {
    return total_rsrc_num;
}

int LinkProject::get_req_num() const {
    return req_num;
}

pair<int, int> LinkProject::get_src_rsrc() const {
    return src_rsrc;
}

pair<int, int> LinkProject::get_dst_rsrc() const {
    return dst_rsrc;
}

pair<int, int> LinkProject::get_edge_rsrc() const {
    return edge_rsrc;
}

LinkGenerator::LinkGenerator(int edge_id, int node_id_a, int node_id_b, double success_rate, PhotonSource* ptn_src):
edge_id(edge_id), node_id_a(node_id_a), node_id_b(node_id_b),
success_rate(success_rate), ptn_src(ptn_src), total_rsrc_num(0), total_req_num(0) {}

LinkGenerator::~LinkGenerator() = default;

int LinkGenerator::get_edge_id() const {
    return edge_id;
}

int LinkGenerator::get_total_rsrc_num() const {
    return total_rsrc_num;
}

int LinkGenerator::get_total_req_num() const {
    return total_req_num;
}

void LinkGenerator::add_rsrc_num(int new_rsrc_num) {
    total_rsrc_num += new_rsrc_num;
}

void LinkGenerator::sub_rsrc_num(int old_rsrc_num) {
    total_rsrc_num -= old_rsrc_num;
}

EntangleLink* LinkGenerator::try_generate_link() const {
    uniform_real_distribution<double> rand_double(0.0,1.0);
    if (rand_double(rand_eng) > success_rate) {
        return nullptr;
    }
    PhotonSource::qubit_num += 2;
    auto new_link = new EntangleLink(node_id_a, node_id_b,
                                     ptn_src->get_fidelity(),
                                     ptn_src->get_fidelity());
    return new_link;
}

vector<EntangleLink*> LinkGenerator::generate_links(int time) const {
    vector<EntangleLink*> new_links;
    for (int i = 0; i < time; i++) {
        for (auto link:new_links) {
            link->add_age(1);
        }
        for (int j = 0; j < total_rsrc_num; j++) {
            EntangleLink* new_link = try_generate_link();
            if (new_link) {
                new_links.push_back(new_link);
            }
        }
    }
    return new_links;
}

void LinkGenerator::add_req_route(int route_id, int req_num) {
    route_requests.emplace(route_id, req_num);
    total_req_num += req_num;
}

void LinkGenerator::del_req_route(int route_id) {
    int queue_size = (int)route_requests.size();
    for (int i = 0; i < queue_size; i++) {
        pair<int, int> req_pair = route_requests.front();
        route_requests.pop();
        if (req_pair.first != route_id) {
            route_requests.push(req_pair);
        }
    }
}

map<int, vector<EntangleLink*>> LinkGenerator::serve_requests(vector<EntangleLink*> new_links) {
    map<int, vector<EntangleLink*>> serve_route_links;
    while (!new_links.empty() && !route_requests.empty()) {
        int next_route_id = route_requests.front().first;
        int next_req_num = route_requests.front().second;
        if (next_req_num <= new_links.size()) {
            serve_route_links[next_route_id].assign(new_links.begin(), new_links.begin()+next_req_num);
            new_links.erase(new_links.begin(), new_links.begin()+next_req_num);
            route_requests.pop();
            total_req_num -= next_req_num;
        } else {
            serve_route_links[next_route_id] = std::move(new_links);
            new_links.clear();
            route_requests.front().second -= (int)serve_route_links[next_route_id].size();
            total_req_num -= (int)serve_route_links[next_route_id].size();
        }
    }
    if (!new_links.empty()) {
        for (auto link:new_links) {
            delete link;
        }
    }
    return serve_route_links;
}

LinkManager::LinkManager(map<int, LinkProject*>& link_projects, int route_id):
link_projects(link_projects), route_id(route_id) {}

LinkManager::~LinkManager() = default;

void LinkManager::print_links() const {
    if (!etg_links.empty()) {
        cout << "Entangle Links ";
    }
    for (const auto& it:etg_links) {
        cout << it.first << "-" << it.second.size() << " ";
    }
    if (!etg_segments.empty()) {
        cout << "Entangle Segments ";
    }
    for (const auto& it:etg_segments) {
        cout << it.first.first << "->" << it.first.second << " ";
    }
}

void LinkManager::add_links(int edge_id, vector<EntangleLink*>& links) {
    etg_links[edge_id].insert(etg_links[edge_id].end(), links.begin(), links.end());
}

map<int, int> LinkManager::update_entangles(int time) {
    // return num of expire links on each edge
    map<int, int> edge_expire_num;
    for (auto& it_etg:etg_links) {
        int edge_id = it_etg.first;
        vector<EntangleLink*>& edge_links = it_etg.second;
        vector<int> expire_link;
        for (int i = 0; i < edge_links.size(); i++) {
            edge_links[i]->add_age(time);
            if (edge_links[i]->is_expired()) {
                expire_link.push_back(i);
            }
        }
        for (int i = (int)expire_link.size() - 1; i >= 0; i--) {
            edge_links.erase(edge_links.begin() + expire_link[i]);
        }
        edge_expire_num[edge_id] = (int)expire_link.size();
    }

    vector<pair<int, int>> expire_segs;
    for (auto it_seg:etg_segments) {
        EntangleSegment* etg_seg = it_seg.second;
        etg_seg->add_age(time);
        if (etg_seg->is_expired()) {
            expire_segs.push_back(it_seg.first);
            for (auto edge:etg_seg->get_path()->get_edges()) {
                int edge_id = edge->get_edge_id();
                if (edge_expire_num.find(edge_id) == edge_expire_num.end()) {
                    edge_expire_num[edge_id] = link_projects[edge_id]->get_req_num();
                } else {
                    edge_expire_num[edge_id] += link_projects[edge_id]->get_req_num();
                }
            }
        }
    }
    for (int i = (int)expire_segs.size() - 1; i >= 0; i--) {
        etg_segments.erase(expire_segs[i]);
    }

    return edge_expire_num;
}

map<int, int> LinkManager::purify_available_links(Path* path, HsRsrcManager* net_rsrc) {
    // return resource released for link generator
    map<int, int> link_generator_release;

    vector<QNode*> path_nodes = path->get_nodes();
    vector<QEdge*> path_edges = path->get_edges();
    for (int i = 0; i < path_nodes.size()-1; i++) {
        int edge_id = path_edges[i]->get_edge_id();
        LinkProject* link_proj = link_projects[edge_id];
        int s_id = path_nodes[i]->get_id();
        int d_id = path_nodes[i+1]->get_id();
        if (etg_links.find(edge_id) != etg_links.end() && etg_links[edge_id].size() == link_proj->get_req_num()) {
            EntangleLink* purified_link = purify_links(etg_links[edge_id]);
            if (purified_link) {
                vector<QNode*> link_nodes = {path_nodes[i], path_nodes[i+1]};
                vector<QEdge*> link_edges = {path_edges[i]};
                Path* link_path = new Path(path_edges[i]->get_success_rate(), link_nodes, link_edges);
                etg_segments[{s_id, d_id}] = new EntangleSegment(purified_link, s_id, d_id, link_path);
                etg_links.erase(edge_id);
                if (RSRC_MANAGE == 1) {
                    if (link_proj->get_src_rsrc().first > 1) {
                        net_rsrc->soft_release_node_memory(s_id, link_proj->get_src_rsrc().first - 1, route_id);
                    }
                    if (link_proj->get_dst_rsrc().first > 1) {
                        net_rsrc->soft_release_node_memory(d_id, link_proj->get_dst_rsrc().first - 1, route_id);
                    }
                    net_rsrc->soft_release_edge_channel(edge_id, link_proj->get_edge_rsrc().first, route_id);
                    link_generator_release[edge_id] += link_proj->get_edge_rsrc().first;
                }
            } else {
                // reclaim the link requirement to link project
                throw logic_error("Purification Fail Not Implement Yet");
            }
        }
    }
    return link_generator_release;
}

map<int, int> LinkManager::swap_all_connected(Path* path, HsRsrcManager* net_rsrc) {
    // return resource recovered for link generator
    map<int, int> link_generator_recover;

    vector<QNode*> path_nodes = path->get_nodes();
    for (int i = 1; i < path_nodes.size()-1; i++) {
        QNode* current_node = path_nodes[i];
        EntangleLink* prev_link = nullptr;
        EntangleLink* next_link = nullptr;
        int s_id;
        int d_id;
        for (auto it_seg:etg_segments) {
            if (it_seg.first.second == current_node->get_id()) {
                prev_link = it_seg.second->get_etg_link();
                s_id = it_seg.first.first;
            }
            if (it_seg.first.first == current_node->get_id()) {
                next_link = it_seg.second->get_etg_link();
                d_id = it_seg.first.second;
            }
        }
        if (prev_link && next_link) {
            Path* prev_path = etg_segments[{s_id, current_node->get_id()}]->get_path();
            int prev_edge_id = prev_path->get_edges().back()->get_edge_id();
            delete etg_segments[{s_id, current_node->get_id()}];
            etg_segments.erase({s_id, current_node->get_id()});
            Path* next_path = etg_segments[{current_node->get_id(), d_id}]->get_path();
            int next_edge_id = next_path->get_edges().front()->get_edge_id();
            delete etg_segments[{current_node->get_id(), d_id}];
            etg_segments.erase({current_node->get_id(), d_id});
            prev_path->append_path(next_path);
            delete next_path;
            Path* new_path = prev_path;
            EntangleLink* new_link = swap_link(prev_link, next_link, current_node);
            if (new_link) {
                etg_segments[{s_id, d_id}] = new EntangleSegment(new_link, s_id, d_id, new_path);
                if (RSRC_MANAGE == 1) {
                    if (link_projects[prev_edge_id]->get_dst_rsrc().first >= 1) {
                        net_rsrc->soft_release_node_memory(current_node->get_id(), 1, route_id);
                    }
                    if (link_projects[next_edge_id]->get_src_rsrc().first >= 1) {
                        net_rsrc->soft_release_node_memory(current_node->get_id(), 1, route_id);
                    }
                }
            } else {
                // travel the new path
                vector<QEdge*> new_edges = new_path->get_edges();
                vector<QNode*> new_nodes = new_path->get_nodes();
                for (int j = 0; j < new_edges.size(); j++) {
                    int edge_id = new_edges[j]->get_edge_id();
                    // preempt the resource
                    if (RSRC_MANAGE == 1) {
                        net_rsrc->preempt_edge_capacity(edge_id, link_projects[edge_id]->get_edge_rsrc().first, route_id);
                        if (j == 0) {
                            net_rsrc->preempt_node_memory(new_nodes[i]->get_id(),
                                                          link_projects[edge_id]->get_src_rsrc().first - 1, route_id);
                        }
                        if (j == new_edges.size()-1) {
                            net_rsrc->preempt_node_memory(new_nodes[i+1]->get_id(),
                                                          link_projects[edge_id]->get_dst_rsrc().first - 1, route_id);
                        } else {
                            net_rsrc->preempt_node_memory(new_nodes[i+1]->get_id(),
                                                          link_projects[edge_id]->get_dst_rsrc().first, route_id);
                        }
                    }
                    // reclaim the link requirement to link project
                    link_generator_recover[edge_id] += link_projects[edge_id]->get_req_num();
                }
//                throw logic_error("Swap Fail Not Implement Yet");
            }
        }
    }

    return link_generator_recover;
}

bool LinkManager::check_user_connection(int s_id, int d_id) {
    return etg_segments.find({s_id, d_id}) != etg_segments.end();
}

EntangleConnection* LinkManager::generate_connection(int s_id, int d_id, HsRsrcManager* net_rsrc) {
    auto* etg_cxn = new EntangleConnection(etg_segments[{s_id, d_id}]);
    delete etg_segments[{s_id, d_id}];
    etg_segments.erase({s_id, d_id});
    return etg_cxn;
}
