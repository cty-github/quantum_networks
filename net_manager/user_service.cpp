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

UserRequest::UserRequest(int request_id, int pair_id, int s_node_id, int d_node_id, double fide_th):
request_id(request_id), pair_id(pair_id),
s_node_id(s_node_id), d_node_id(d_node_id),
fide_th(fide_th), request_time(get_current_time()),
created(false), closed(false) {}

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

bool UserRequest::has_created() const {
    return created;
}

void UserRequest::set_created() {
    created = true;
    satisfy_time = get_current_time();
}

bool UserRequest::has_closed() const {
    return closed;
}

void UserRequest::set_closed() {
    closed = true;
}

ClockTime UserRequest::get_request_time() const {
    return request_time;
}

ClockTime UserRequest::get_satisfy_time() const {
    return satisfy_time;
}

int UserRequest::get_next_id() {
    return next_id;
}

void UserRequest::add_next_id() {
    next_id++;
}

void UserRequest::print_request() const {
    cout << "Request " << request_id << ": SD " << s_node_id << "->" << d_node_id << " Fide " << fide_th << endl;
}

int UserConnection::next_id = 0;

UserConnection::UserConnection(EntangleConnection* etg_cxn, int connection_id, int request_id, int route_id):
etg_cxn(etg_cxn), connection_id(connection_id), request_id(request_id), route_id(route_id),
created_time(get_current_time()), finished(false) {}

UserConnection::~UserConnection() = default;

int UserConnection::get_s_node_id() const {
    return etg_cxn->get_s_id();
}

int UserConnection::get_d_node_id() const {
    return etg_cxn->get_d_id();
}

double UserConnection::get_fidelity() const {
    return etg_cxn->get_fidelity();
}

int UserConnection::get_age() const {
    return etg_cxn->get_age();
}

void UserConnection::add_age(int time) {
    etg_cxn->add_age(time);
}

bool UserConnection::is_expired() const {
    return etg_cxn->is_expired();
}

int UserConnection::get_connection_id() const {
    return connection_id;
}

int UserConnection::get_request_id() const {
    return request_id;
}

int UserConnection::get_route_id() const {
    return route_id;
}

ClockTime UserConnection::get_created_time() const {
    return created_time;
}

bool UserConnection::is_finished() const {
    return finished;
}

int UserConnection::get_next_id() {
    return next_id;
}

void UserConnection::add_next_id() {
    next_id++;
}

void UserConnection::finish_connection() {
    finished = true;
//    cout << "Finish Connection " << connection_id << " of Request " << request_id << endl;
    delete etg_cxn;
}
