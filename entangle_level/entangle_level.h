//
// Created by TianyaoChu on 2023/5/7.
//

#ifndef QUANTUM_NETWORKS_ENTANGLE_LEVEL_H
#define QUANTUM_NETWORKS_ENTANGLE_LEVEL_H

//#include "qelement.h"
#include "entangle_params.h"
#include "net_topology/net_topology.h"
#include "net_topology/path.h"

class EntangleLink {
private:
    int node_id_a;
    int node_id_b;
    double x_fidelity;
    double z_fidelity;
    int age;
public:
//    EntangleLink(int node_id_a, int node_id_b,
//                 double src_bit_flip, double dst_bit_flip,
//                 double src_phase_flip, double dst_phase_flip);
    EntangleLink(int node_id_a, int node_id_b, double x_fidelity, double z_fidelity);
    EntangleLink(int node_id_a, int node_id_b, double x_fidelity, double z_fidelity, int age);
    ~EntangleLink();
    int get_node_id_a() const;
    int get_node_id_b() const;
    double get_x_fidelity() const;
    double get_z_fidelity() const;
    double get_fidelity() const;
    int get_age() const;
    void add_age(int time);
    bool is_expired() const;
    friend EntangleLink* purify_links(vector<EntangleLink*> links);
    friend EntangleLink* swap_link(EntangleLink* prev_link, EntangleLink* next_link, QNode* node);
};

class EntangleSegment {
private:
    EntangleLink* etg_link;
    int s_id;
    int d_id;
    Path* path;
public:
    EntangleSegment(EntangleLink* etg_link, int s_id, int d_id, Path* path);
    explicit EntangleSegment(EntangleSegment* etg_seg);
    ~EntangleSegment();
    EntangleLink* get_etg_link() const;
    int get_s_id() const;
    int get_d_id() const;
    double get_x_fidelity() const;
    double get_z_fidelity() const;
    double get_fidelity() const;
    int get_age() const;
    void add_age(int time);
    bool is_expired() const;
    Path* get_path() const;
};

class EntangleConnection: public EntangleSegment{
public:
    explicit EntangleConnection(EntangleSegment* etg_seg);
    ~EntangleConnection();
    void print_etg_cxn() const;
};

#endif //QUANTUM_NETWORKS_ENTANGLE_LEVEL_H
