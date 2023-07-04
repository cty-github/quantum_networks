//
// Created by TianyaoChu on 2023/5/7.
//

#ifndef QUANTUM_NETWORKS_USER_SERVICE_H
#define QUANTUM_NETWORKS_USER_SERVICE_H

#include "qdevice/qdevice.h"
#include "entangle_level/entangle_level.h"
#include "utils/timing.h"

class SDPair {
private:
    int pair_id;
    int s_node_id;
    int d_node_id;
    double fide_th;
    static int next_id;
public:
    SDPair(int pair_id, int s_node_id, int d_node_id, double fide_th);
    ~SDPair();
    int get_pair_id() const;
    int get_s_node_id() const;
    int get_d_node_id() const;
    double get_fide_th() const;
    static int get_next_id();
    static void add_next_id();
};

class UserRequest {
private:
    int request_id;
    int pair_id;
    int s_node_id;
    int d_node_id;
    double fide_th;
    bool created;  // connection created
    bool closed;   // connection closed
    ClockTime request_time;
    ClockTime satisfy_time;  // connection created
    static int next_id;
public:
    UserRequest(int request_id, int pair_id, int s_node_id, int d_node_id, double fide_th);
    ~UserRequest();
    int get_request_id() const;
    int get_pair_id() const;
    int get_s_node_id() const;
    int get_d_node_id() const;
    double get_fide_th() const;
    bool has_created() const;
    void set_created();
    bool has_closed() const;
    void set_closed();
    ClockTime get_request_time() const;
    ClockTime get_satisfy_time() const;
    static int get_next_id();
    static void add_next_id();
    void print_request() const;
};

class UserConnection {
private:
    EntangleConnection* etg_cxn;
    int connection_id;
    int request_id;
    int route_id;
    ClockTime created_time;
    bool finished;
    static int next_id;
public:
    UserConnection(EntangleConnection* etg_cxn, int connection_id, int request_id, int route_id);
    ~UserConnection();
    int get_s_node_id() const;
    int get_d_node_id() const;
    double get_fidelity() const;
    int get_age() const;
    void add_age(int time);
    bool is_expired() const;
    int get_connection_id() const;
    int get_request_id() const;
    int get_route_id() const;
    ClockTime get_created_time() const;
    bool is_finished() const;
    static int get_next_id();
    static void add_next_id();
    void finish_connection();
};

#endif //QUANTUM_NETWORKS_USER_SERVICE_H
