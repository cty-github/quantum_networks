//
// Created by TianyaoChu on 2023/5/7.
//

#ifndef QUANTUM_NETWORKS_LINK_MANAGER_H
#define QUANTUM_NETWORKS_LINK_MANAGER_H

#include "user_service.h"
#include "net_resource.h"
#include "net_topology/net_topology.h"
#include "net_topology/path.h"
#include "entangle_level/entangle_level.h"
#include <vector>
#include <queue>

class LinkProject {
private:
    int s_node_id;
    int d_node_id;
    int edge_id;    // edge the link on
    int route_id;   // route the link for
    int total_rsrc_num;   // total resource for the project
    int req_num;    // link num the route need
    pair<int, int> src_rsrc;  // hs rsrc for src node
    pair<int, int> dst_rsrc;  // hs rsrc for dst node
    pair<int, int> edge_rsrc;  // hs rsrc for edge
public:
    int src_preempted;
    int dst_preempted;
    int edge_preempted;
public:
    LinkProject(int s_node_id, int d_node_id, int edge_id, int route_id, int total_rsrc_num, int req_num,
                pair<int, int> src_rsrc, pair<int, int> dst_rsrc, pair<int, int> edge_rsrc);
    ~LinkProject();
    int get_s_node_id() const;
    int get_d_node_id() const;
    int get_edge_id() const;
    int get_route_id() const;
    int get_total_rsrc_num() const;
    int get_req_num() const;
    pair<int, int> get_src_rsrc() const;
    pair<int, int> get_dst_rsrc() const;
    pair<int, int> get_edge_rsrc() const;
};

class LinkGenerator {   // public among all route projects
private:
    int edge_id;
    int node_id_a;
    int node_id_b;
    double success_rate;
    PhotonSource* ptn_src;
    int max_capacity;
    int total_rsrc_num;
    int total_req_num;
    queue<pair<int, int>> route_requests; // queue of request route id and request num
public:
    LinkGenerator(QEdge* edge);
    ~LinkGenerator();
    int get_edge_id() const;
    int get_total_rsrc_num() const;
    int get_total_req_num() const;
    void add_rsrc_num(int new_rsrc_num);
    void sub_rsrc_num(int old_rsrc_num);
    void add_route_req(int route_id, int req_num);
    EntangleLink* try_generate_link() const;
    vector<EntangleLink*> generate_links(int time) const;
    map<int, vector<EntangleLink*>> serve_requests(vector<EntangleLink*> new_links);
};

class LinkManager { // for each route project to manage its links
private:
    map<int, LinkProject*> link_projects;   // map between edge id and link proj on it
    map<int, vector<EntangleLink*>> etg_links;  // map between edge id and links on it
    map<pair<int, int>, EntangleSegment*> etg_segments;
    int route_id;
public:
    LinkManager(map<int, LinkProject*>& link_projects, int route_id);
    ~LinkManager();
    void print_links() const;
    void add_links(int edge_id, vector<EntangleLink*>& links);
    RouteUpdate update_entangles(int time, HsRsrcManager* net_rsrc=nullptr);
    RouteUpdate purify_available_links(Path* path, HsRsrcManager* net_rsrc=nullptr);
    RouteUpdate swap_all_connected(Path* path, HsRsrcManager* net_rsrc=nullptr);
    bool check_user_connection(int s_id, int d_id);
    EntangleConnection* generate_connection(int s_id, int d_id);
};

#endif //QUANTUM_NETWORKS_LINK_MANAGER_H
