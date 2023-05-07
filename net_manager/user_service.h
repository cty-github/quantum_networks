//
// Created by TianyaoChu on 2023/5/7.
//

#ifndef QUANTUM_NETWORKS_USER_SERVICE_H
#define QUANTUM_NETWORKS_USER_SERVICE_H

#include "qdevice/qdevice.h"
#include "entangle_level/entangle_level.h"

class UserRequest {
private:
    int s_node_id;
    int d_node_id;
    double fide_th;
    int connect_num;
public:
    UserRequest(int s_node_id, int d_node_id, double fide_th, int connect_num);
    ~UserRequest();
    void print_request() const;
};

class UserConnection {
private:
    EntangleConnection* etg_cxn{};
    int s_node_id;
    int d_node_id;
    double fidelity;
public:
    UserConnection();
    ~UserConnection();
};

#endif //QUANTUM_NETWORKS_USER_SERVICE_H
