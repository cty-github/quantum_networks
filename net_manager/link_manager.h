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
    int edge_id;
    int rsrc_num;
    UserRequest* request;
public:
    LinkProject(int s_node_id, int d_node_id, int edge_id, int rsrc_num, UserRequest* request);
    ~LinkProject();
    int get_s_node_id() const;
    int get_d_node_id() const;
    int get_edge_id() const;
    int get_rsrc_num() const;
    UserRequest* get_request() const;
};

class LinkGenerator {
private:
    int edge_id;
    int total_rsrc_num;
    queue<LinkProject*> request_projects;
public:
    explicit LinkGenerator(int edge_id);
    ~LinkGenerator();
};

class LinkManager {
private:
    map<int, LinkGenerator*> link_generators;
    vector<EntangleLink*> etg_link_pool;
    NetTopology* net_topo;
public:
    explicit LinkManager(NetTopology* net_topo);
    ~LinkManager();
    EntangleLink* try_generate_link(int node_id_a, int node_id_b) const;
    EntangleLink* try_connect_link(EntangleLink* src_link, EntangleLink* dst_link) const;
    vector<EntangleLink*> generate_links(Path* path) const;
    EntangleConnection* connect_links(const vector<EntangleLink*>& links, Path* path);
};

#endif //QUANTUM_NETWORKS_LINK_MANAGER_H
