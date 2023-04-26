//
// Created by TianyaoChu on 2023/4/13.
//

#ifndef QUANTUM_NETWORKS_NET_MANAGER_H
#define QUANTUM_NETWORKS_NET_MANAGER_H

#include "net_topology/net_topology.h"
#include "net_topology/routing.h"
#include "qdevice/qdevice.h"
#include <utility>

class EntangleRoute {
private:
    EntangleLink* etg_link;
    Path* path;
    NetTopology* net_topo;
public:
    EntangleRoute(EntangleLink* etg_link, Path* path, NetTopology* net_topo);
    ~EntangleRoute();
    EntangleLink* get_etg_link() const;
    void print_etg_link() const;
    Path* get_path() const;
    NetTopology* get_net_topo() const;
};

class LinkManager {
private:
    vector<EntangleLink*> etg_link_pool;
    NetTopology* net_topo;
public:
    explicit LinkManager(NetTopology* net_topo);
    ~LinkManager();
    vector<EntangleLink*> generate_links(Path* path);
    EntangleRoute* connect_links(const vector<EntangleLink*>& links, Path* path);
};

class LinkProject {
private:
    vector<pair<int, int>> requests;
public:
    LinkProject();
    ~LinkProject();
};

#endif //QUANTUM_NETWORKS_NET_MANAGER_H
