//
// Created by TianyaoChu on 2023/4/13.
//

#include "net_manager.h"
#include "utils/tool.h"
#include <iostream>

EntangleRoute::EntangleRoute(EntangleLink* etg_link, Routing* route, NetTopology* net_topo):
etg_link(etg_link), route(route), net_topo(net_topo) {}

EntangleRoute::~EntangleRoute() = default;

EntangleLink* EntangleRoute::get_etg_link() const {
    return etg_link;
}

void EntangleRoute::print_etg_link() const {
    cout << "Entangle Connection: Node " << etg_link->get_src_id() << " <-> Node " << etg_link->get_dst_id() << endl;
    cout << "X Fidelity: " << etg_link->get_x_fidelity() << endl;
    cout << "Z Fidelity: " << etg_link->get_z_fidelity() << endl;
    cout << "Bit Fidelity: " << etg_link->get_bit_fidelity() << endl;
}

Routing* EntangleRoute::get_route() const {
    return route;
}

NetTopology* EntangleRoute::get_net_topo() const {
    return net_topo;
}

LinkManager::LinkManager(NetTopology* net_topo): net_topo(net_topo) {}

LinkManager::~LinkManager() = default;

vector<EntangleLink*> LinkManager::generate_links(Routing* route) {
    vector<EntangleLink*> etg_links = {};
    for (auto node:route->get_path()) {
        int node_id_a = node->get_id();
        if (node_id_a == route->get_end_node_id()) {
            continue;
        }
        QNode* node_b = route->get_next_node(node_id_a);
        int node_id_b = node_b->get_id();
        QEdge* edge = net_topo->get_edge(node_id_a, node_id_b);
        double a_x = node->get_pos_x();
        double a_y = node->get_pos_y();
        double b_x = node_b->get_pos_x();
        double b_y = node_b->get_pos_y();
        double p_x = edge->get_ptn_src()->get_pos_x();
        double p_y = edge->get_ptn_src()->get_pos_y();
        double dist_a = dist(a_x, a_y, p_x, p_y);
        double dist_b = dist(b_x, b_y, p_x, p_y);
        etg_links.push_back(edge->get_ptn_src()->generate_link(node_id_a, node_id_b, dist_a, dist_b));
    }
    return etg_links;
}

EntangleRoute* LinkManager::connect_links(const vector<EntangleLink*>& links, Routing* route) {
    cout << "Fidelity: ";
    EntangleLink* etg_link = nullptr;
    for (auto link:links) {
        if (!etg_link) {
            etg_link = link;
            cout << etg_link->get_fidelity();
        } else {
            int repeater_id = etg_link->get_dst_id();
            if (link->get_src_id() != repeater_id) {
                cout << "Error: No Sequential Links" << endl;
                return nullptr;
            }
            EntangleLink* old_etg_link = etg_link;
            BSM* bsm = net_topo->get_node(repeater_id)->get_bsm();
            etg_link = bsm->connect_link(old_etg_link, link);
            delete old_etg_link;
            delete link;
            cout << " -> " << etg_link->get_fidelity();
        }
    }
    cout << endl;
    return new EntangleRoute(etg_link, route, net_topo);
}
