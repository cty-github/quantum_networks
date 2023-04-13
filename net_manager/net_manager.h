//
// Created by TianyaoChu on 2023/4/13.
//

#ifndef QUANTUM_NETWORKS_NET_MANAGER_H
#define QUANTUM_NETWORKS_NET_MANAGER_H

#include "net_topology/net_topology.h"
#include "net_topology/routing.h"
#include "qdevice/qdevice.h"

class EntangleRoute {
private:
    EntangleLink* etg_link;
    Routing* route;
    NetTopology* net_topo;
public:
    EntangleRoute(EntangleLink* etg_link, Routing* route, NetTopology* net_topo);
    ~EntangleRoute();
    EntangleLink* get_etg_link() const;
    void print_etg_link() const;
    Routing* get_route() const;
    NetTopology* get_net_topo() const;
};

class LinkManager {
private:
    vector<EntangleLink*> etg_link_pool;
    NetTopology* net_topo;
public:
    explicit LinkManager(NetTopology* net_topo);
    ~LinkManager();
    vector<EntangleLink*> generate_links(Routing* route);
    EntangleRoute* connect_links(const vector<EntangleLink*>& links, Routing* route);
};

#endif //QUANTUM_NETWORKS_NET_MANAGER_H
