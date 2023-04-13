//
// Created by TianyaoChu on 2023/4/5.
//

#include "routing.h"
#include <iostream>

Routing::Routing(double cost, vector<QNode*>& path):cost(cost), path(path), visit(false) {}

Routing::Routing(const Routing& routing) {
    cost = routing.cost;
    path = routing.path;
    visit = routing.visit;
}

double Routing::get_cost() const {
    return cost;
}

void Routing::set_cost(double new_cost) {
    cost = new_cost;
}

double Routing::combine_cost(double cost_a, double cost_b) {
    return cost_a * cost_b;
}

vector<QNode*> Routing::get_path() const {
    return path;
}

void Routing::set_path(vector<QNode*>& new_path) {
    path = new_path;
}

bool Routing::get_visit() const {
    return visit;
}

void Routing::set_visit() {
    visit = true;
}

int Routing::get_next_node_id(int node_id) const {
    for (int i = 0; i < path.size(); i++) {
        if (path[i]->get_id() == node_id) {
            if (i+1 < path.size()) {
                return path[i+1]->get_id();
            } else {
                return -1;
            }
        }
    }
    return -1;
}

int Routing::get_start_node_id() const {
    return path.front()->get_id();
}

int Routing::get_end_node_id() const {
    return path.back()->get_id();
}

void Routing::append_node(QNode* new_node, double new_cost) {
    path.push_back(new_node);
    cost = combine_cost(cost, new_cost);
}

void Routing::print_routing() const {
    cout << "cost: " << cost << " path:";
    for (auto node:path) {
        cout << " " << node->get_id();
    }
    cout << endl;
}

void Routing::insert_routing(Routing* precede_route) {
    cost = combine_cost(precede_route->get_cost(), cost);
    vector<QNode*> pre_path = precede_route->get_path();
    for (int i = 0; i < pre_path.size() - 1; i++) {
        path.insert(path.begin()+i, pre_path[i]);
    }
}

void Routing::append_routing(Routing* follow_route) {
    cost = combine_cost(cost, follow_route->get_cost());
    vector<QNode*> follow_path = follow_route->get_path();
    for (int i = 1; i < follow_path.size(); i++) {
        path.push_back(follow_path[i]);
    }
}
