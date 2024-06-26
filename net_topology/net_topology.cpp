//
// Created by TianyaoChu on 2023/3/29.
//

#include "net_topology.h"
#include "test_params.h"
#include "utils/rand.h"
#include "utils/tool.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <utility>
#include <queue>

NetTopology::NetTopology(): node_num(0), edge_num(0) {}

NetTopology::NetTopology(DeviceManager* dev_mgr, int user_num, int repeater_num,
                         double size, double alpha, double beta):
node_num(0), edge_num(0) {
    uniform_int_distribution<int> rand_mem(MEM_LOW, MEM_UP);
    uniform_int_distribution<int> rand_cap(CAP_LOW, CAP_UP);
    uniform_real_distribution<double> rand_double(0.0,1.0);
    for (int i = 0; i < user_num + repeater_num; i++) {
        NodeType node_type;
        if (i < user_num) {
            node_type = User;
        } else {
            node_type = Repeater;
        }
        double pos_x = size*rand_double(rand_eng);
        double pos_y = size*rand_double(rand_eng);
        BSM* bsm = dev_mgr->get_closest_bsm(pos_x, pos_y);
        add_node(i, node_type, pos_x, pos_y, rand_mem(rand_eng), bsm);
    }
    for (int i = 0; i < node_num; i++) {
        for (int j = i+1; j < node_num; j++) {
            if (rand_double(rand_eng) < beta * exp(-get_distance(i, j)/(alpha*size))) {
                double i_x = nodes[i]->get_pos_x();
                double i_y = nodes[i]->get_pos_y();
                double j_x = nodes[j]->get_pos_x();
                double j_y = nodes[j]->get_pos_y();
                PhotonSource* ptn_src = dev_mgr->get_closest_ptn_src((i_x + j_x) / 2, (i_y + j_y) / 2);
                double ptn_x = ptn_src->get_pos_x();
                double ptn_y = ptn_src->get_pos_y();
                double p_i = exp(-ptn_src->get_decay_rate()*dist(i_x, i_y, ptn_x, ptn_y));
                double p_j = exp(-ptn_src->get_decay_rate()*dist(j_x, j_y, ptn_x, ptn_y));
                double success_rate = p_i * p_j;
                add_edge(edge_num, i, j, rand_cap(rand_eng),
                         get_distance(i, j), success_rate, ptn_src);
            }
        }
    }
}

NetTopology::NetTopology(const string& filepath, DeviceManager* dev_mgr): node_num(0), edge_num(0) {
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
            int bsm_id;
            ss >> key;
            if (key == "user") {
                node_type = User;
            } else if (key == "repeater") {
                node_type = Repeater;
            } else {
                node_type = Unknown;
                cout << "Error in File" << endl;
            }
            ss >> id >> pos_x >> pos_y >> memory_size >> bsm_id;
            add_node(id, node_type, pos_x, pos_y, memory_size, dev_mgr->get_bsm(bsm_id));
        } else if (key == "edge") {
            int edge_id;
            int node_id_a;
            int node_id_b;
            int capacity;
            int ptn_src_id;
            ss >> edge_id >> node_id_a >> node_id_b >> capacity >> ptn_src_id;
            double a_x = nodes[node_id_a]->get_pos_x();
            double a_y = nodes[node_id_a]->get_pos_y();
            double b_x = nodes[node_id_b]->get_pos_x();
            double b_y = nodes[node_id_b]->get_pos_y();
            PhotonSource* ptn_src = dev_mgr->get_ptn_src(ptn_src_id);
            double ptn_x = ptn_src->get_pos_x();
            double ptn_y = ptn_src->get_pos_y();
            double p_a = exp(-ptn_src->get_decay_rate()*dist(a_x, a_y, ptn_x, ptn_y));
            double p_b = exp(-ptn_src->get_decay_rate()*dist(b_x, b_y, ptn_x, ptn_y));
            double success_rate = p_a * p_b;
            add_edge(edge_id, node_id_a, node_id_b,
                     capacity, get_distance(node_id_a, node_id_b),
                     success_rate, ptn_src);
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

bool NetTopology::add_node(int id, NodeType node_type, double pos_x, double pos_y, int memory_size, BSM* bsm) {
    if (nodes.find(id) != nodes.end()) {
        cout << "Exist Node " << id << endl;
        return false;
    }
    auto* new_node = new QNode(id, node_type, pos_x, pos_y, memory_size, bsm);
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
    return dist(nodes.find(node_id_a)->second->get_pos_x(),
                nodes.find(node_id_a)->second->get_pos_y(),
                nodes.find(node_id_b)->second->get_pos_x(),
                nodes.find(node_id_b)->second->get_pos_y());
}

bool NetTopology::add_edge(int edge_id, int node_id_a, int node_id_b, int capacity,
                           double distance, double success_rate, PhotonSource* ptn_src) {
    if (nodes.find(node_id_a) == nodes.end()) {
        cout << "No Node " << node_id_a << endl;
        return false;
    } else if (nodes.find(node_id_b) == nodes.end()) {
        cout << "No Node " << node_id_b << endl;
        return false;
    }
    if (edges_node[node_id_a].find(node_id_b) != edges_node[node_id_a].end()) {
        cout << "Exist Edge between " << node_id_a << " and " << node_id_b << endl;
        return false;
    } else if (node_id_a == node_id_b) {
        cout << "No Loop in the Net" << endl;
        return false;
    }
    auto* new_edge = new QEdge(edge_id, node_id_a, node_id_b, capacity, distance, success_rate, ptn_src);
    edges_node[node_id_a][node_id_b] = new_edge;
    edges_node[node_id_b][node_id_a] = new_edge;
    edges_id[edge_id] = new_edge;
    nodes[node_id_a]->add_adjacent_node(node_id_b);
    nodes[node_id_b]->add_adjacent_node(node_id_a);
    edge_num++;
    return true;
}

bool NetTopology::add_edge(const QEdge &edge) {
    int node_id_a = edge.get_node_id_a();
    int node_id_b = edge.get_node_id_b();
    int edge_id = edge.get_edge_id();
    if (nodes.find(node_id_a) == nodes.end()) {
        cout << "No Node " << node_id_a << endl;
        return false;
    } else if (nodes.find(node_id_b) == nodes.end()) {
        cout << "No Node " << node_id_b << endl;
        return false;
    }
    if (edges_node[node_id_a].find(node_id_b) != edges_node[node_id_a].end()) {
        cout << "Exist Edge between " << node_id_a << " and " << node_id_b << endl;
        return false;
    }
    auto* new_edge = new QEdge(edge);
    edges_node[node_id_a][node_id_b] = new_edge;
    edges_node[node_id_b][node_id_a] = new_edge;
    edges_id[edge_id] = new_edge;
    nodes[node_id_a]->add_adjacent_node(node_id_b);
    nodes[node_id_b]->add_adjacent_node(node_id_a);
    edge_num++;
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
    if (edges_node[node_id_a].find(node_id_b) == edges_node[node_id_a].end()) {
        return nullptr;
    }
    return edges_node[node_id_a][node_id_b];
}

QEdge* NetTopology::get_edge(int edge_id) {
    if (edges_id.find(edge_id) == edges_id.end()) {
        return nullptr;
    }
    return edges_id[edge_id];
}

map<int, QEdge *> NetTopology::get_edges() {
    return edges_id;
}

bool NetTopology::save_topo(const string& filepath) const {
    ofstream file;
    file.open(filepath,ios::out);
    if (!file.is_open()) {
        cout << "Cannot Open File " << filepath << endl;
        return false;
    }
    file << setprecision(6);
    file << "#\t\ttype\t\tid\tpos_x\tpos_y\tn_mem\tbsm" << endl;
    for (auto & it : nodes) {
        QNode* node = it.second;
        file << "node\t";
        if (node->is_user()) {
            file << "user\t\t";
        } else if (node->is_repeater()) {
            file << "repeater\t";
        } else {
            file << "unknown\t";
        }
        file << node->get_id() << '\t';
        file << node->get_pos_x() << '\t';
        file << node->get_pos_y() << '\t';
        file << node->get_memory_size() << "\t\t";
        file << node->get_bsm()->get_id() << endl;
    }
    file << endl;
    file << "#\t\tid\t\tnode_A\tnode_B\tcap\tptn_src" << endl;
    for (auto & it_a : edges_node) {
        for (auto & it_b : it_a.second) {
            QEdge* edge = it_b.second;
            if (it_a.first < it_b.first) {
                file << "edge\t";
                file << edge->get_edge_id() << "\t\t";
                file << edge->get_node_id_a() << "\t\t";
                file << edge->get_node_id_b() << "\t\t";
                file << edge->get_channel_capacity() << '\t';
                file << edge->get_ptn_src()->get_id() << endl;
            }
        }
    }
    file << endl;
    file.close();
    return true;
}

Path* NetTopology::get_path(int src_node_id, int dst_node_id,
                            const set<int>& closed_nodes,
                            const set<int>& closed_edge_nodes) {
    if (get_node(src_node_id) == nullptr) {
        cout << "No Src Node " << src_node_id << endl;
        return nullptr;
    } else if (get_node(dst_node_id) == nullptr) {
        cout << "No Dst Node " << dst_node_id << endl;
        return nullptr;
    }

    // get the shortest path between src and dst with dijkstra algorithm
    map<int, Path*> src_path;
    vector<QNode*> nodes_0 = {get_node(src_node_id)};
    vector<QEdge*> edges_0 = {};
    src_path[src_node_id] = new Path(1, nodes_0, edges_0);

    priority_queue<pair<double, Path*>, vector<pair<double, Path*>>, Path::cmp_path> alter_path;
    alter_path.emplace(1, src_path[src_node_id]);

    while (!alter_path.empty()) {
        double current_cost = alter_path.top().first;
        int current_node = alter_path.top().second->get_end_node_id();
        alter_path.pop();
        if (src_path[current_node]->get_visit()) {
            continue;
        }
//        cout << "current " << current_node << " " << current_cost << endl;
        src_path[current_node]->set_visit();
        if (current_node != src_node_id && get_node(current_node)->is_user()) {
            // avoid passing through user nodes
            continue;
        }
        for (auto adj_node:get_node(current_node)->get_adjacent_nodes_id()) {
            if (closed_nodes.find(adj_node) != closed_nodes.end()) {
                continue;
            }
            if (current_node == src_node_id && closed_edge_nodes.find(adj_node) != closed_edge_nodes.end()) {
                continue;
            }
            double new_cost = Path::combine_cost(
                    Path::combine_cost(
                            current_cost,
                            get_node(current_node)->get_success_rate()),
                    get_edge(current_node, adj_node)->get_success_rate());
//            cout << "new " << i << " " << new_cost << endl;
            vector<QNode*> new_nodes = src_path[current_node]->get_nodes();
            new_nodes.push_back(get_node(adj_node));
            vector<QEdge*> new_edges = src_path[current_node]->get_edges();
            new_edges.push_back(get_edge(current_node, adj_node));
            if (src_path.find(adj_node) == src_path.end()) {
                src_path[adj_node] = new Path(new_cost, new_nodes, new_edges);
                alter_path.emplace(new_cost, src_path[adj_node]);
            } else if (!src_path[adj_node]->get_visit() && new_cost > src_path[adj_node]->get_cost()) {
                src_path[adj_node]->set_cost(new_cost);
                src_path[adj_node]->set_nodes(new_nodes);
                src_path[adj_node]->set_edges(new_edges);
                alter_path.emplace(new_cost, src_path[adj_node]);
            }
        }
    }

    if (src_path.find(dst_node_id) == src_path.end()) {
//        cout << "No Route between " << node_id_a << " and " << node_id_b << endl;
        return nullptr;
    }
    return src_path[dst_node_id];
}

vector<Path*> NetTopology::get_paths(int src_node_id, int dst_node_id, int k) {
    if (get_node(src_node_id) == nullptr) {
        cout << "No Src Node " << src_node_id << endl;
        return {};
    } else if (get_node(dst_node_id) == nullptr) {
        cout << "No Dst Node " << dst_node_id << endl;
        return {};
    }

    // get k shortest paths between src and dst with ksp algorithm
    Path* shortest_path = get_path(src_node_id, dst_node_id);
    if (!shortest_path) {
//        cout << "No Route between " << node_id_a << " and " << node_id_b << endl;
        return {};
    }
    priority_queue<pair<double, Path*>, vector<pair<double, Path*>>, Path::cmp_path> alter_path;
    alter_path.emplace(shortest_path->get_cost(), shortest_path);

    vector<Path*> k_paths;
    while (!alter_path.empty() && k_paths.size() < k) {
        Path* expand_path = alter_path.top().second;
        alter_path.pop();
        k_paths.push_back(expand_path);
        Path* precede_path;
        set<int> closed_nodes;
        for (auto dev_node:expand_path->get_nodes()) {
            int dev_id = dev_node->get_id();
            if (dev_id == dst_node_id) {
                continue;
            } else if (dev_id == src_node_id) {
                vector<QNode*> nodes_0 = {get_node(src_node_id)};
                vector<QEdge*> edges_0 = {};
                precede_path = new Path(1, nodes_0, edges_0);
            } else {
                QEdge* dev_edge = get_edge(precede_path->get_end_node_id(),
                                           dev_id);
                double new_cost = dev_edge->get_success_rate();
                precede_path->append_node(dev_node, dev_edge, new_cost);
            }
            set<int> closed_edge_nodes;
            for (auto path:k_paths) {
                int next_node_id = path->get_next_node_id(dev_id);
                if (next_node_id >= 0) {
                    closed_edge_nodes.emplace(next_node_id);
                }
            }
            Path* dev_path = get_path(dev_node->get_id(), dst_node_id,
                                      closed_nodes, closed_edge_nodes);
            if (dev_path) {
                dev_path->insert_path(precede_path);
                alter_path.emplace(dev_path->get_cost(), dev_path);
            }
            closed_nodes.emplace(dev_id);
        }
    }
    return k_paths;
}
