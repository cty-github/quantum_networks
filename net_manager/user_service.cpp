//
// Created by TianyaoChu on 2023/5/7.
//

#include "user_service.h"
#include <iostream>

int SDPair::next_id = 0;

SDPair::SDPair(int pair_id, int s_node_id, int d_node_id, double fide_th):
pair_id(pair_id), s_node_id(s_node_id), d_node_id(d_node_id), fide_th(fide_th) {}

SDPair::~SDPair() = default;

int SDPair::get_pair_id() const {
    return pair_id;
}

int SDPair::get_s_node_id() const {
    return s_node_id;
}

int SDPair::get_d_node_id() const {
    return d_node_id;
}

double SDPair::get_fide_th() const {
    return fide_th;
}

int SDPair::get_next_id() {
    return next_id;
}

void SDPair::add_next_id() {
    next_id++;
}

int UserRequest::next_id = 0;

UserRequest::UserRequest(int request_id, int pair_id,
                         int s_node_id, int d_node_id,
                         double fide_th, int request_num):
request_id(request_id), pair_id(pair_id),
s_node_id(s_node_id), d_node_id(d_node_id),
fide_th(fide_th), request_num(request_num) {}

UserRequest::~UserRequest() = default;

int UserRequest::get_request_id() const {
    return request_id;
}

int UserRequest::get_pair_id() const {
    return pair_id;
}

int UserRequest::get_s_node_id() const {
    return s_node_id;
}

int UserRequest::get_d_node_id() const {
    return d_node_id;
}

double UserRequest::get_fide_th() const {
    return fide_th;
}

int UserRequest::get_request_num() const {
    return request_num;
}

int UserRequest::get_next_id() {
    return next_id;
}

void UserRequest::add_next_id() {
    next_id++;
}

void UserRequest::print_request() const {
    cout << s_node_id << "->" << d_node_id << ": " << fide_th;
}

UserConnection::UserConnection(EntangleConnection* etg_cxn, int s_node_id, int d_node_id, double fidelity):
etg_cxn(etg_cxn), s_node_id(s_node_id), d_node_id(d_node_id), fidelity(fidelity) {}

UserConnection::~UserConnection() = default;
