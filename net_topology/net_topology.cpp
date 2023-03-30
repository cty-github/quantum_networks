//
// Created by TianyaoChu on 2023/3/29.
//

#include "net_topology.h"
#include "utils/rand.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <utility>
#include <queue>

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

NetTopology::NetTopology(): node_num(0), edge_num(0) {}

NetTopology::NetTopology(int user_num, int repeater_num, double size, double alpha, double beta):
        node_num(0), edge_num(0) {
    uniform_int_distribution<int> rand_int(10,14);
    uniform_real_distribution<double> rand_double(0.0,1.0);
    for (int i = 0; i < user_num + repeater_num; i++) {
        if (i < user_num) {
            add_node(i, User,
                     size*rand_double(rand_eng),
                     size*rand_double(rand_eng),
                     rand_int(rand_eng), 0.9);
        } else {
            add_node(i, Repeater,
                     size*rand_double(rand_eng),
                     size*rand_double(rand_eng),
                     rand_int(rand_eng), 0.9);
        }
    }
    for (int i = 0; i < node_num; i++) {
        for (int j = i+1; j < node_num; j++) {
            if (rand_double(rand_eng) < beta * exp(-get_distance(i, j)/(alpha*size))) {
                add_edge(i, j, rand_int(rand_eng),
                         get_distance(i, j), 0.0002);
            }
        }
    }
}

NetTopology::NetTopology(const string& filepath): node_num(0), edge_num(0) {
    ifstream file;
    file.open(filepath,ios::in);
    if (!file.is_open()) {
        cout << "Cannot Open File " << filepath << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        stringstream ss(line);
        string key;
        ss >> key;
        if (key == "node") {
            NodeType node_type;
            int id;
            double pos_x;
            double pos_y;
            int memory_size;
            double success_probability;
            ss >> key;
            if (key == "user") {
                node_type = User;
            } else if (key == "repeater") {
                node_type = Repeater;
            } else {
                node_type = Unknown;
                cout << "Error in File" << endl;
            }
            ss >> id >> pos_x >> pos_y >> memory_size >> success_probability;
            add_node(id, node_type, pos_x, pos_y, memory_size, success_probability);
        } else if (key == "edge") {
            int node_id_a;
            int node_id_b;
            int capacity;
            double decay_rate;
            ss >> node_id_a >> node_id_b >> capacity >> decay_rate;
            add_edge(node_id_a, node_id_b, capacity, get_distance(node_id_a, node_id_b), decay_rate);
        } else {
            cout << "Error in File" << endl;
        }
    }
    file.close();
}

NetTopology::~NetTopology() = default;

int NetTopology::get_node_num() const {
    return node_num;
}

int NetTopology::get_edge_num() const {
    return edge_num;
}

bool NetTopology::add_node(int id, NodeType node_type, double pos_x, double pos_y,
                           int memory_size, double success_probability) {
    if (nodes.find(id) != nodes.end()) {
        cout << "Exist Node " << id << endl;
        return false;
    }
    auto* new_node = new QNode(id, node_type, pos_x, pos_y, memory_size, success_probability);
    nodes[id] = new_node;
    node_num++;
    return true;
}

bool NetTopology::add_node(const QNode &node) {
    int id = node.get_id();
    if (nodes.find(id) != nodes.end()) {
        cout << "Exist Node " << id << endl;
        return false;
    }
    auto* new_node = new QNode(node);
    nodes[id] = new_node;
    node_num++;
    return true;
}

double NetTopology::get_distance(int node_id_a, int node_id_b) const {
    if (nodes.find(node_id_a) == nodes.end()) {
        cout << "No Node " << node_id_a << endl;
        return 0;
    } else if (nodes.find(node_id_b) == nodes.end()) {
        cout << "No Node " << node_id_b << endl;
        return 0;
    }
    double distance = sqrt(pow((nodes.find(node_id_a)->second->get_pos_x()
                                - nodes.find(node_id_b)->second->get_pos_x()), 2)
                           + pow((nodes.find(node_id_a)->second->get_pos_y()
                                  - nodes.find(node_id_b)->second->get_pos_y()), 2));
    return distance;
}

bool NetTopology::add_edge(int node_id_a, int node_id_b, int capacity,
                           double distance, double decay_rate) {
    if (nodes.find(node_id_a) == nodes.end()) {
        cout << "No Node " << node_id_a << endl;
        return false;
    } else if (nodes.find(node_id_b) == nodes.end()) {
        cout << "No Node " << node_id_b << endl;
        return false;
    }
    if (edges[node_id_a].find(node_id_b) != edges[node_id_a].end()) {
        cout << "Exist Edge between " << node_id_a << " and " << node_id_b << endl;
        return false;
    } else if (node_id_a == node_id_b) {
        cout << "No Loop in the Net" << endl;
        return false;
    }
    auto* new_edge = new QEdge(node_id_a, node_id_b, capacity, distance, decay_rate);
    edges[node_id_a][node_id_b] = new_edge;
    edges[node_id_b][node_id_a] = new_edge;
    edge_num++;
    nodes[node_id_a]->add_adjacent_node(node_id_b);
    nodes[node_id_b]->add_adjacent_node(node_id_a);
    return true;
}

bool NetTopology::add_edge(const QEdge &edge) {
    int node_id_a = edge.get_node_id_a();
    int node_id_b = edge.get_node_id_b();
    if (nodes.find(node_id_a) == nodes.end()) {
        cout << "No Node " << node_id_a << endl;
        return false;
    } else if (nodes.find(node_id_b) == nodes.end()) {
        cout << "No Node " << node_id_b << endl;
        return false;
    }
    if (edges[node_id_a].find(node_id_b) != edges[node_id_a].end()) {
        cout << "Exist Edge between " << node_id_a << " and " << node_id_b << endl;
        return false;
    }
    auto* new_edge = new QEdge(edge);
    edges[node_id_a][node_id_b] = new_edge;
    edges[node_id_b][node_id_a] = new_edge;
    edge_num++;
    nodes[node_id_a]->add_adjacent_node(node_id_b);
    nodes[node_id_b]->add_adjacent_node(node_id_a);
    return true;
}

QNode* NetTopology::get_node(int node_id) {
    if (nodes.find(node_id) == nodes.end()) {
        return nullptr;
    }
    return nodes[node_id];
}

QEdge* NetTopology::get_edge(int node_id_a, int node_id_b) {
    if (nodes.find(node_id_a) == nodes.end() || nodes.find(node_id_b) == nodes.end()) {
        return nullptr;
    }
    if (edges[node_id_a].find(node_id_b) == edges[node_id_a].end()) {
        return nullptr;
    }
    return edges[node_id_a][node_id_b];
}

bool NetTopology::save_net(const string& filepath) const {
    ofstream file;
    file.open(filepath,ios::out);
    if (!file.is_open()) {
        cout << "Cannot Open File " << filepath << endl;
        return false;
    }
    cout << "Save QNetwork to File " << filepath << endl;
    for (auto & it : nodes) {
        QNode* node = it.second;
        file << "node\t";
        if (node->is_user()) {
            file << "user\t";
        } else if (node->is_repeater()) {
            file << "repeater\t";
        } else {
            file << "unknown\t";
        }
        file << node->get_id() << '\t';
        file << node->get_pos_x() << '\t';
        file << node->get_pos_y() << '\t';
        file << node->get_memory_size() << '\t';
        file << node->get_success_probability() << endl;
    }
    for (auto & it_a : edges) {
        for (auto & it_b : it_a.second) {
            QEdge* edge = it_b.second;
            if (edge->get_node_id_a() < edge->get_node_id_b()) {
                file << "edge\t";
                file << edge->get_node_id_a() << '\t';
                file << edge->get_node_id_b() << '\t';
                file << edge->get_capacity() << '\t';
                file << edge->get_decay_rate() << endl;
            }
        }
    }
    file.close();
    return true;
}

Routing* NetTopology::get_routing(int src_node_id, int dst_node_id) {
    if (get_node(src_node_id) == nullptr) {
        cout << "No Src Node " << src_node_id << endl;
        return nullptr;
    } else if (get_node(dst_node_id) == nullptr) {
        cout << "No Dst Node " << dst_node_id << endl;
        return nullptr;
    }

    // get the shortest routing between src and dst with dijkstra algorithm
    map<int, Routing*> src_route;
    vector<QNode*> path_0 = {get_node(src_node_id)};
    src_route[src_node_id] = new Routing(0, path_0);

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> unvisited_node = {};
    unvisited_node.emplace(0, src_node_id);

    while (!unvisited_node.empty()) {
        double current_cost = unvisited_node.top().first;
        int current_node = unvisited_node.top().second;
        unvisited_node.pop();
        if (src_route[current_node]->get_visit()) {
            continue;
        }
//        cout << "current " << current_node << " " << current_cost << endl;
        src_route[current_node]->set_visit();
        if (current_node != src_node_id && get_node(current_node)->is_user()) {
            continue;
        }
        for (auto i:get_node(current_node)->get_adjacent_nodes_id()) {
            double new_cost = current_cost + get_edge(current_node, i)->get_distance();
//            cout << "new " << i << " " << new_cost << endl;
            vector<QNode*> new_path = src_route[current_node]->get_path();
            new_path.push_back(get_node(i));
            if (src_route.find(i) == src_route.end()) {
                src_route[i] = new Routing(new_cost, new_path);
                unvisited_node.emplace(new_cost, i);
            } else if (!src_route[i]->get_visit() && new_cost < src_route[i]->get_cost()) {
                src_route[i]->set_cost(new_cost);
                src_route[i]->set_path(new_path);
                unvisited_node.emplace(new_cost, i);
            }
        }
    }

    if (src_route.find(dst_node_id) == src_route.end()) {
        cout << "No Route between " << src_node_id << " and " << dst_node_id << endl;
    }
    return src_route[dst_node_id];
}

vector<Routing*> NetTopology::get_routings(int src_node_id, int dst_node_id, int k) {
    cout << k << endl;
    return {get_routing(src_node_id, dst_node_id)};
}
