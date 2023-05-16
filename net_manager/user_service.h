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
    int request_num;
    int processed_num;
    int served_num;
    ClockTime start_time;
    bool finished;  // process finished
    ClockTime finish_time;  // finish of process
    bool end;   // all connections of request end
    static int next_id;
public:
    UserRequest(int request_id, int pair_id, int s_node_id, int d_node_id, double fide_th, int request_num);
    ~UserRequest();
    int get_request_id() const;
    int get_pair_id() const;
    int get_s_node_id() const;
    int get_d_node_id() const;
    double get_fide_th() const;
    int get_request_num() const;
    void add_processed_num(int num);
    void add_served_num(int num);
    ClockTime get_start_time() const;
    bool is_finished() const;
    bool has_end() const;
    static int get_next_id();
    static void add_next_id();
    void print_request() const;
};

class UserConnection {
private:
    EntangleConnection* etg_cxn;
    int connection_id;
    int request_id;
    ClockTime created_time;
    bool finished;
    static int next_id;
public:
    UserConnection(EntangleConnection* etg_cxn, int connection_id, int request_id);
    ~UserConnection();
    int get_s_node_id() const;
    int get_d_node_id() const;
    double get_fidelity() const;
    int get_age() const;
    void add_age(int time);
    bool is_expired() const;
    int get_connection_id() const;
    int get_request_id() const;
    ClockTime get_created_time() const;
    bool is_finished() const;
    static int get_next_id();
    static void add_next_id();
    void finish_connection();
};

#endif //QUANTUM_NETWORKS_USER_SERVICE_H
