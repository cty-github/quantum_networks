//
// Created by TianyaoChu on 2023/5/7.
//

#include "link_manager.h"
#include "utils/rand.h"
#include "utils/tool.h"
#include <cmath>
#include <iostream>

LinkProject::LinkProject(int s_node_id, int d_node_id, int edge_id, int rsrc_num, UserRequest* request):
        s_node_id(s_node_id), d_node_id(d_node_id), edge_id(edge_id), rsrc_num(rsrc_num), request(request) {}

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

int LinkProject::get_rsrc_num() const {
    return rsrc_num;
}

UserRequest* LinkProject::get_request() const {
    return request;
}

LinkGenerator::LinkGenerator(int edge_id): edge_id(edge_id), total_rsrc_num(0) {}

LinkGenerator::~LinkGenerator() = default;

LinkManager::LinkManager(NetTopology* net_topo): net_topo(net_topo) {}

LinkManager::~LinkManager() = default;

EntangleLink* LinkManager::try_generate_link(int node_id_a, int node_id_b) const {
    QNode* node_a = net_topo->get_node(node_id_a);
    QNode* node_b = net_topo->get_node(node_id_b);
    PhotonSource* ptn_src = net_topo->get_edge(node_id_a, node_id_b)->get_ptn_src();
    double a_x = node_a->get_pos_x();
    double a_y = node_a->get_pos_y();
    double b_x = node_b->get_pos_x();
    double b_y = node_b->get_pos_y();
    double p_x = ptn_src->get_pos_x();
    double p_y = ptn_src->get_pos_y();
    double dist_a = dist(a_x, a_y, p_x, p_y);
    double dist_b = dist(b_x, b_y, p_x, p_y);
    double p_a = exp(-ptn_src->get_decay_rate()*dist_a);
    double p_b = exp(-ptn_src->get_decay_rate()*dist_b);
    uniform_real_distribution<double> rand_double(0.0,1.0);
    if (rand_double(rand_eng) > p_a * p_b) {
        return nullptr;
    }
    PhotonSource::qubit_num += 2;
    return new EntangleLink(node_id_a, node_id_b,
                            ptn_src->get_fidelity(), ptn_src->get_fidelity());
}

EntangleLink* LinkManager::try_connect_link(EntangleLink* src_link, EntangleLink* dst_link) const {
    if (src_link->get_dst_id() != dst_link->get_src_id()) {
        cout << "Error: Not the Same Repeater" << endl;
        return nullptr;
    }
    int repeater_id = src_link->get_dst_id();
    BSM* bsm = net_topo->get_node(repeater_id)->get_bsm();

    int new_src_id = src_link->get_src_id();
    int new_dst_id = dst_link->get_dst_id();
    double src_x_f = src_link->get_x_fidelity();
    double src_z_f = src_link->get_z_fidelity();
    double dst_x_f = dst_link->get_x_fidelity();
    double dst_z_f = dst_link->get_z_fidelity();

    double new_x_fidelity = src_x_f * dst_x_f + (1-src_x_f) * (1-dst_x_f);
    new_x_fidelity = new_x_fidelity * bsm->get_x_fidelity() + (1-new_x_fidelity) * (1-bsm->get_x_fidelity());
    double new_z_fidelity = src_z_f * dst_z_f + (1-src_z_f) * (1-dst_z_f);
    new_z_fidelity = new_z_fidelity * bsm->get_z_fidelity() + (1-new_z_fidelity) * (1-bsm->get_z_fidelity());

    uniform_real_distribution<double> rand_double(0.0,1.0);
    if (rand_double(rand_eng) > bsm->get_success_rate()) {
        cout << "Connect Links Fail" << endl;
        return nullptr;
    }
    return new EntangleLink(new_src_id, new_dst_id,
                            new_x_fidelity, new_z_fidelity);
}

vector<EntangleLink*> LinkManager::generate_links(Path* path) const {
    vector<EntangleLink*> etg_links = {};
    for (auto node_a:path->get_nodes()) {
        int node_id_a = node_a->get_id();
        if (node_id_a == path->get_end_node_id()) {
            continue;
        }
        QNode* node_b = path->get_next_node(node_id_a);
        int node_id_b = node_b->get_id();
        etg_links.push_back(try_generate_link(node_id_a, node_id_b));
        node_a->occupy_memory(1);
        node_b->occupy_memory(1);
    }
    return etg_links;
}

EntangleConnection* LinkManager::connect_links(const vector<EntangleLink*>& links, Path* path) {
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
            etg_link = try_connect_link(old_etg_link, link);
            if (!etg_link) {
                cout << "Connect Links Fail" << endl;
                return nullptr;
            }
            delete old_etg_link;
            delete link;
            cout << " -> " << etg_link->get_fidelity();
            net_topo->get_node(repeater_id)->release_memory(2);
        }
    }
    cout << endl;
    return new EntangleConnection(etg_link, path, net_topo);
}
