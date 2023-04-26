//
// Created by TianyaoChu on 2023/4/5.
//

#include "routing.h"
#include <iostream>

Path::Path(double cost, vector<QNode*>& nodes): cost(cost), nodes(nodes), visit(false) {}

Path::Path(const Path& path) {
    cost = path.cost;
    nodes = path.nodes;
    visit = path.visit;
}

double Path::get_cost() const {
    return cost;
}

void Path::set_cost(double new_cost) {
    cost = new_cost;
}

double Path::combine_cost(double cost_a, double cost_b) {
    return cost_a * cost_b;
}

vector<QNode*> Path::get_nodes() const {
    return nodes;
}

void Path::set_nodes(vector<QNode*>& new_nodes) {
    nodes = new_nodes;
}

bool Path::get_visit() const {
    return visit;
}

void Path::set_visit() {
    visit = true;
}

QNode* Path::get_next_node(int node_id) const {
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->get_id() == node_id) {
            if (i+1 < nodes.size()) {
                return nodes[i + 1];
            } else {
                return nullptr;
            }
        }
    }
    return nullptr;
}

int Path::get_next_node_id(int node_id) const {
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->get_id() == node_id) {
            if (i+1 < nodes.size()) {
                return nodes[i + 1]->get_id();
            } else {
                return -1;
            }
        }
    }
    return -1;
}

int Path::get_start_node_id() const {
    return nodes.front()->get_id();
}

int Path::get_end_node_id() const {
    return nodes.back()->get_id();
}

void Path::append_node(QNode* new_node, double new_cost) {
    nodes.push_back(new_node);
    cost = combine_cost(cost, new_cost);
}

void Path::print_path() const {
    cout << "Path: " ;
    for (int i = 0; i < nodes.size(); i++) {
        if (i == 0) {
            cout << nodes[i]->get_id();
        } else {
            cout << " -> " << nodes[i]->get_id();
        }
    }
    cout << endl;
//    cout << "Fidelity: " << cost << endl;
}

void Path::insert_path(Path* precede_path) {
    cost = combine_cost(precede_path->get_cost(), cost);
    vector<QNode*> precede_nodes = precede_path->get_nodes();
    for (int i = 0; i < precede_nodes.size() - 1; i++) {
        nodes.insert(nodes.begin() + i, precede_nodes[i]);
    }
}

void Path::append_path(Path* follow_path) {
    cost = combine_cost(cost, follow_path->get_cost());
    vector<QNode*> follow_nodes = follow_path->get_nodes();
    for (int i = 1; i < follow_nodes.size(); i++) {
        nodes.push_back(follow_nodes[i]);
    }
}
