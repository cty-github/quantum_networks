//
// Created by TianyaoChu on 2023/5/7.
//

#include "user_service.h"
#include <iostream>

UserRequest::UserRequest(int s_node_id, int d_node_id, double fide_th, int connect_num):
s_node_id(s_node_id), d_node_id(d_node_id), fide_th(fide_th), connect_num(connect_num) {}

UserRequest::~UserRequest() = default;

void UserRequest::print_request() const {
    cout << s_node_id << "->" << d_node_id << ": " << fide_th << ", " << connect_num << endl;
}

UserConnection::UserConnection() {}

UserConnection::~UserConnection() = default;
