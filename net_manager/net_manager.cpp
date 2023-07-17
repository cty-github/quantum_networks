//
// Created by TianyaoChu on 2023/4/13.
//

#include "net_manager.h"
#include "test_params.h"
#include "utils/rand.h"
#include <ilconcert/iloenv.h>
#include <ilconcert/ilomodel.h>
#include <ilcplex/ilocplex.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

NetManager::NetManager(NetTopology* net_topo, int user_num): net_topo(net_topo) {
    uniform_real_distribution<double> rand_fidelity(SD_FIDE_LOW,SD_FIDE_UP);
    int sd_num = (int)(SD_RATIO * user_num);
    uniform_int_distribution<int> rand_id(0, user_num-1);
    int gen_num = 0;
    while(gen_num < sd_num) {
        int i = rand_id(rand_eng);
        int j = rand_id(rand_eng);
        if (i != j) {
            sd_pairs[SDPair::get_next_id()] = new SDPair(SDPair::get_next_id(),
                                                         i, j,
                                                         rand_fidelity(rand_eng));
            SDPair::add_next_id();
            gen_num++;
        }
    }
//    double request_rate = SD_RATIO/user_num;
//    uniform_real_distribution<double> rand_double(0.0,1.0);
//    for (int i = 0; i < user_num; i++) {
//        for (int j = 0; j < user_num; j++) {
//            if (i != j && rand_double(rand_eng) < request_rate) {
//                sd_pairs[SDPair::get_next_id()] = new SDPair(SDPair::get_next_id(),
//                                                             i, j,
//                                                             rand_fidelity(rand_eng));
//                SDPair::add_next_id();
//            }
//        }
//    }
    switch (RSRC_MANAGE) {
        case 0: {
            net_rsrc = new BasicRsrcManager(net_topo);
            break;
        }
        case 1: {
            net_rsrc = new HsRsrcManager(net_topo);
            break;
        }
        default: {
            net_rsrc = new BasicRsrcManager(net_topo);
            break;
        }
    }
    route_manager = new RouteManager(net_topo);
}

NetManager::NetManager(const string& filepath, NetTopology *net_topo): net_topo(net_topo) {
    load_sd_pairs(filepath);
    switch (RSRC_MANAGE) {
        case 0: {
            net_rsrc = new BasicRsrcManager(net_topo);
            break;
        }
        case 1: {
            net_rsrc = new HsRsrcManager(net_topo);
            break;
        }
        default: {
            net_rsrc = new BasicRsrcManager(net_topo);
            break;
        }
    }
    route_manager = new RouteManager(net_topo);
}

NetManager::~NetManager() = default;

int NetManager::get_sd_num() const {
    return (int)sd_pairs.size();
}

void NetManager::load_sd_pairs(const string& filepath) {
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
        if (key == "sd_pair") {
            int pair_id;
            int s_id;
            int d_id;
            double fidelity;
            ss >> pair_id >> s_id >> d_id >> fidelity;
            sd_pairs[pair_id] = new SDPair(pair_id, s_id, d_id, fidelity);
        } else {
            cout << "Error in File" << endl;
        }
    }
    file.close();
}

bool NetManager::save_sd_pairs(const string& filepath) const {
    ofstream file;
    file.open(filepath,ios::out);
    if (!file.is_open()) {
        cout << "Cannot Open File " << filepath << endl;
        return false;
    }
    file << setprecision(6);
    file << "#\t\tpair_id\ts_id\td_id\tfide_th" << endl;
    for (auto & it:sd_pairs) {
        SDPair* sd_pair = it.second;
        file << "sd_pair\t";
        file << sd_pair->get_pair_id() << "\t\t";
        file << sd_pair->get_s_node_id() << "\t\t";
        file << sd_pair->get_d_node_id() << "\t\t";
        file << sd_pair->get_fide_th() << endl;
    }
    file << endl;
    file.close();
    return true;
}

Path* NetManager::get_path(int src_node_id, int dst_node_id) {
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    return net_topo->get_path(src_node_id, dst_node_id);
}

vector<Path*> NetManager::get_paths(int src_node_id, int dst_node_id, int k) {
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    return net_topo->get_paths(src_node_id, dst_node_id, k);
}

bool NetManager::initialize(int k) {
    // prepare the shortest k paths as candidate paths for each request pair
    for (auto it:sd_pairs) {
        SDPair* sd_pair = it.second;
        candidate_paths[it.first]=get_paths(sd_pair->get_s_node_id(),
                                            sd_pair->get_d_node_id(), k);
//        cout << "Shortest Route from " << sd_pair->get_s_node_id()
//             << " to " << sd_pair->get_d_node_id() << endl;
//        for (auto path:candidate_paths[it.first]) {
//            path->print_path();
//            cout << endl;
//        }
//        cout << endl;
    }
    return true;
}

int NetManager::get_waiting_num() const {
    return (int)waiting_requests.size();
}

void NetManager::print_waiting_requests() const {
    cout << "- Waiting Requests" << endl;
//    for (auto request:waiting_requests) {
//        request.second->print_request();
//    }
    cout << "Waiting Num: " << get_waiting_num() << endl;
}

int NetManager::get_processing_num() const {
    return (int)processing_requests.size();
}

void NetManager::print_processing_requests() const {
    cout << "- Processing Requests" << endl;
//    for (auto request:processing_requests) {
//        request.second->print_request();
//    }
    cout << "Processing Num: " << get_processing_num() << endl;
}

int NetManager::get_serving_num() const {
    return (int)serving_requests.size();
}

void NetManager::print_serving_requests() const {
    cout << "- Serving Requests" << endl;
//    for(auto request:serving_requests) {
//        request.second->print_request();
//    }
    cout << "Serving Num: " << get_serving_num() << endl;
}

void NetManager::print_routing_projects() const {
    route_manager->print_routing_projects();
}

int NetManager::get_user_cxn_num() const {
    return (int)user_connections.size();
}

void NetManager::print_user_connections() const {
    cout << "- User Connections" << endl;
//    for(const auto& it_cxn:user_connections) {
//        int request_id = it_cxn.first;
//        UserConnection* cxn = it_cxn.second;
//        cout << "Connection " << cxn->get_connection_id() << " Serve Request " << request_id << endl;
//    }
    cout << "User Connection Num: " << get_user_cxn_num() << endl;
}

map<int, int> NetManager::get_req_delay() const {
    return req_delay;
}

vector<UserRequest*> NetManager::random_request(int time, double time_prob, double sd_prob, double req_rate) {
    if (net_topo == nullptr) {
        cout << "No Net Topology" << endl;
        return {};
    }
    uniform_real_distribution<double> rand_double(0.0,1.0);
    vector<UserRequest*> random_requests;
    for (int i = 0; i < time; i++) {
        if (rand_double(rand_eng) > time_prob) {
            continue;
        }
        for (auto it:sd_pairs) {
            SDPair* sd_pair = it.second;
            if (rand_double(rand_eng) < sd_prob) {
                int s_id = sd_pair->get_s_node_id();
                int d_id = sd_pair->get_d_node_id();
                double fide_th = sd_pair->get_fide_th();
                int max_request = (int)(std::sqrt(net_topo->get_node(s_id)->get_memory_size() *
                                                  net_topo->get_node(d_id)->get_memory_size()) * req_rate);
                uniform_int_distribution<int> rand_int(0,max_request);
                int num_request = rand_int(rand_eng);
                while (num_request > 0) {
                    auto* user_request = new UserRequest(UserRequest::get_next_id(), it.first, s_id, d_id, fide_th);
                    UserRequest::add_next_id();
                    random_requests.push_back(user_request);
                    num_request--;
                }
            }
        }
    }
    return random_requests;
}

void NetManager::add_new_requests(const vector<UserRequest*>& new_requests) {
    for (auto request:new_requests) {
        user_requests[request->get_request_id()] = request;
        waiting_requests[request->get_request_id()] = request;
    }
}

void NetManager::reserve_resource(RouteProject* route_proj) {
//    cout << "Reserve Resource of Route " << route_proj->get_route_id() << endl;
    for (auto it:route_proj->get_link_projs()) {
        LinkProject* link_proj = it.second;
        net_rsrc->reserve_link_resource(link_proj->get_s_node_id(),
                                        link_proj->get_d_node_id(),
                                        link_proj->get_total_rsrc_num(),
                                        route_proj->get_route_id());
    }
}

//modified by Li
vector<RouteProject*> NetManager::static_routing_projects(RsrcManager* tmp_rsrc) {
    vector<RouteProject*> new_route_projects;
    if(waiting_requests_ordered.empty()){
        //cout<< "Empty waiting requests.\n";
        return new_route_projects;
    }
    int routed_request_num = 0;
    for (auto it_wait:waiting_requests_ordered) {
        //UserRequest* serve_request = it_wait.second;
        UserRequest* serve_request = it_wait;
        int pair_id = serve_request->get_pair_id();
        bool found_available_path = 0;
        for (auto path:candidate_paths.find(pair_id)->second) { //直接从备选path中选一个判断是否合适
            vector<QEdge*> edges = path->get_edges();
            int rsrc_num;
            for (int i = 0; i < edges.size(); i++) { //这一整个for循环就是为了找到这个path中的最小rsrc_num
                int node_id_a = edges[i]->get_node_id_a();
                int node_id_b = edges[i]->get_node_id_b();
                int max_num = tmp_rsrc->max_link_resource(node_id_a, node_id_b);
                if (i == 0) {
                    rsrc_num = max_num;
                }
                if (max_num < rsrc_num) {
                    rsrc_num = max_num;
                }
            }
            if (rsrc_num) { //如果合适的话
                auto* route_proj = new RouteProject(RouteProject::get_next_id(),
                                                    rsrc_num, path, serve_request);
                RouteProject::add_next_id();
                new_route_projects.push_back(route_proj);
                for (auto it:route_proj->get_link_projs()) {
                    LinkProject* link_proj = it.second;
                    tmp_rsrc->reserve_link_resource(link_proj->get_s_node_id(),
                                                    link_proj->get_d_node_id(),
                                                    link_proj->get_total_rsrc_num(),
                                                    route_proj->get_route_id());
                }
                found_available_path = 1;
                break;
            }
        }
        if(!found_available_path)
            break;
        routed_request_num += 1;
    }
    waiting_requests_ordered.erase(waiting_requests_ordered.begin(),waiting_requests_ordered.begin() + routed_request_num);
    return new_route_projects;
}

vector<RouteProject*> NetManager::greedy_routing_projects(RsrcManager* tmp_rsrc) const {
    // greedy realization for test
    vector<RouteProject*> new_route_projects;
    for (auto it_wait:waiting_requests) {
        UserRequest* serve_request = it_wait.second;
        int pair_id = serve_request->get_pair_id();
        for (auto path:candidate_paths.find(pair_id)->second) {
            vector<QEdge*> edges = path->get_edges();
            int rsrc_num;
            for (int i = 0; i < edges.size(); i++) {
                int node_id_a = edges[i]->get_node_id_a();
                int node_id_b = edges[i]->get_node_id_b();
                int max_num = tmp_rsrc->max_link_resource(node_id_a, node_id_b);
                if (i == 0) {
                    rsrc_num = max_num;
                }
                if (max_num < rsrc_num) {
                    rsrc_num = max_num;
                }
            }
            if (rsrc_num) {
                auto* route_proj = new RouteProject(RouteProject::get_next_id(),
                                                    rsrc_num, path, serve_request);
                RouteProject::add_next_id();
                new_route_projects.push_back(route_proj);
                for (auto it:route_proj->get_link_projs()) {
                    LinkProject* link_proj = it.second;
                    tmp_rsrc->reserve_link_resource(link_proj->get_s_node_id(),
                                                    link_proj->get_d_node_id(),
                                                    link_proj->get_total_rsrc_num(),
                                                    route_proj->get_route_id());
                }
                break;
            }
        }
    }
    return new_route_projects;
}

vector<RouteProject *> NetManager::heuristic_routing_projects(RsrcManager *tmp_rsrc) const {
    // solve as optimal problem
    vector<RouteProject*> new_route_projects;

    // vectorize waiting requests map
    int num_requests = (int)waiting_requests.size();
    vector<int> request_ids;
    request_ids.reserve(waiting_requests.size());
    for (auto it: waiting_requests) {
        request_ids.push_back(it.first);
    }

    // get the number of selected requests
    int num_select = NUM_SELECTED_REQUESTS > num_requests ? num_requests : NUM_SELECTED_REQUESTS;
    double max_obj_value = 0;
    for (int k = 0; k < NUM_REPEATS; k++) {
        auto* tmp_rsrc_copy = new BasicRsrcManager((BasicRsrcManager*)tmp_rsrc);
        int current_route_id = RouteProject::get_next_id();
        shuffle(request_ids.begin(), request_ids.end(), rand_eng);
        double obj_value = 0;
        vector<RouteProject*> current_route_projects;
        for (int i = 0; i < num_select; ++i) {
            UserRequest* serve_request = waiting_requests.find(request_ids[i])->second;
            int pair_id = serve_request->get_pair_id();
            for (auto path:candidate_paths.find(pair_id)->second) {
                vector<QEdge*> edges = path->get_edges();
                int rsrc_num;
                for (int j = 0; j < edges.size(); j++) {
                    int node_id_a = edges[j]->get_node_id_a();
                    int node_id_b = edges[j]->get_node_id_b();
                    int max_num = tmp_rsrc_copy->max_link_resource(node_id_a, node_id_b);
                    if (j == 0) {
                        rsrc_num = max_num;
                    }
                    if (max_num < rsrc_num) {
                        rsrc_num = max_num;
                    }
                }
                if (rsrc_num) {
                    auto* route_proj = new RouteProject(current_route_id,
                                                        rsrc_num, path, serve_request);
                    current_route_id++;
                    current_route_projects.push_back(route_proj);
                    for (auto it:route_proj->get_link_projs()) {
                        LinkProject* link_proj = it.second;
                        tmp_rsrc_copy->reserve_link_resource(link_proj->get_s_node_id(),
                                                             link_proj->get_d_node_id(),
                                                             link_proj->get_total_rsrc_num(),
                                                             route_proj->get_route_id());
                    }
                    obj_value += calculate_obj(serve_request, route_proj);
                    break;
                }
            }
        }
        if (max_obj_value < obj_value) {
            max_obj_value = obj_value;
            new_route_projects = current_route_projects;
        }
    }
    for (auto it_rp: new_route_projects) {
        RouteProject::add_next_id();
        for (auto it: it_rp->get_link_projs()) {
            LinkProject* link_proj = it.second;
            tmp_rsrc->reserve_link_resource(link_proj->get_s_node_id(),
                                            link_proj->get_d_node_id(),
                                            link_proj->get_total_rsrc_num(),
                                            it_rp->get_route_id());
        }
    }
    return new_route_projects;
}

//vector<RouteProject*> NetManager::optimal_routing_projects(RsrcManager* tmp_rsrc) const {
//    vector<RouteProject*> new_route_projects;
//
//    const int num_requests {int(waiting_requests.size())};
//    const int num_paths {CANDIDATE_NUM};
//    const int max_rsrc_num {100};
//
//    vector<UserRequest*> requests;
//    vector<vector<Path*>> paths;
//    vector<int> edges_id;
//    vector<int> nodes_id;
//    vector<double> edge_success_rates;
//    vector<int> edge_rsrc_nums;
//    vector<int> node_rsrc_nums;
//    vector<vector<vector<unsigned short>>> have_edges;
//    vector<vector<vector<unsigned short>>> have_nodes;
//
//    const auto num_edges = edges_id.size();
//    const auto num_nodes = nodes_id.size();
//
//    for (auto it_wait: waiting_requests) {
//        auto serve_request = it_wait.second;
//        requests.push_back(serve_request);
//        for (auto path: candidate_paths.find(serve_request->get_pair_id())->second) {
//            paths[requests.size() - 1].push_back(path);
//            for (auto edge: path->get_edges()) {
//                if (!binary_search(edges_id.begin(), edges_id.end(), edge->get_edge_id())) {
//                    edges_id.push_back(edge->get_edge_id());
//                    edge_success_rates.push_back(edge->get_success_rate());
//                    edge_rsrc_nums.push_back(edge->get_channel_capacity());
//                }
//                if (!binary_search(nodes_id.begin(), nodes_id.end(), edge->get_node_id_a())) {
//                    nodes_id.push_back(edge->get_node_id_a());
//                    node_rsrc_nums.push_back(net_topo->get_node(edge->get_node_id_a())->get_memory_size());
//                }
//                if (!binary_search(nodes_id.begin(), nodes_id.end(), edge->get_node_id_b())) {
//                    nodes_id.push_back(edge->get_node_id_b());
//                    node_rsrc_nums.push_back(net_topo->get_node(edge->get_node_id_b())->get_memory_size());
//                }
//            }
//        }
//    }
//
//    for (int i = 0; i < num_requests; ++i) {
//        have_edges.emplace_back();
//        for (int j = 0; j < num_paths; ++j) {
//            have_edges[i].emplace_back();
//            auto path = paths[i][j];
//            have_edges[i][j] = vector<unsigned short>(edges_id.size(), 0);
//            auto edges = path->get_edges();
//            auto path_edges_size = edges.size();
//            for (auto k = 0; k < path_edges_size; ++k) {
//                auto it_node_a_pos = find(nodes_id.begin(), nodes_id.end(), edges[k]->get_node_id_a());
//                auto it_node_b_pos = find(nodes_id.begin(), nodes_id.end(), edges[k]->get_node_id_b());
//                auto it_edge_pos = find(edges_id.begin(), edges_id.end(), edges[k]->get_edge_id());
//                if (k == 0) {
//                    have_nodes[i][j][it_node_a_pos - nodes_id.begin()] = 1;
//                    have_nodes[i][j][it_node_b_pos - nodes_id.begin()] = 2;
//                    have_edges[i][j][it_edge_pos - edges_id.begin()] = 1;
//                } else if (k == path_edges_size - 1) {
//                    have_nodes[i][j][it_node_a_pos - nodes_id.begin()] = 2;
//                    have_nodes[i][j][it_node_b_pos - nodes_id.begin()] = 1;
//                    have_edges[i][j][it_edge_pos - edges_id.begin()] = 1;
//                } else {
//                    have_nodes[i][j][it_node_a_pos - nodes_id.begin()] = 2;
//                    have_nodes[i][j][it_node_b_pos - nodes_id.begin()] = 2;
//                    have_edges[i][j][it_edge_pos - edges_id.begin()] = 1;
//                }
//            }
//        }
//    }
//    // 到此初始化全部完成.....
//
//    // cplex求解部分
//    IloEnv env;//创建环境变量
//    IloModel model(env);//创建模型对象
//
//    // 声明变量
//    // 变量1，satisfy，选择第i个request，第j条候选路径
//    IloArray<IloBoolVarArray> satisfy(env);
//    satisfy.setSize(num_requests);
//    for (int i = 0; i < satisfy.getSize(); ++i) {
//        satisfy[i].setSize(num_paths);
//    }
//    for (int i = 0; i < satisfy.getSize(); i++) {
//        for (int j = 0; j < satisfy[i].getSize(); j++) {
//            satisfy[i][j] = IloBoolVar(env);
//        }
//    }
//
//    // 变量2，received_rsrc，给每个请求分配多少资源
//    IloIntVarArray received_rsrc(env);
//    received_rsrc.setSize(num_requests);
//    for (int i = 0; i < num_requests; ++i) {
//        received_rsrc[i] = IloIntVar(env, 0, max_rsrc_num);
//    }
//
//    // 目标函数
//    IloExpr e(env);
//    for (int i = 0; i < num_requests; ++i) {
//        IloExpr e1(env); // 计算等候时间 * satisfy
//        IloExpr e2(env); // 计算下部
//
//        IloExpr e_sum(env);
//        int num_e = 0;
//        for (int j = 0; j < num_paths; ++j) {
//            e_sum += satisfy[i][j];
//            IloExpr e_prod(env);
//            for (int k = 0; k < num_edges; ++k) {
//                if (have_edges[i][j][k]) {
//                    e_prod = e_prod * edge_success_rates[k] * received_rsrc[i];
//                    num_e++;
//                }
//            }
//            e2 += satisfy[i][j] * e_prod;
//        }
//
//        e1 += get_time_interval(get_current_time(), requests[i]->get_request_time()) * e_sum;
//        e2 = 1 / (pow(LIFESPAN, num_e - 1) * e2 + 0.1);
//        e = e1 / e2;
//    }
//    model.add(IloMaximize(env, e));
//    e.end();
//    // 约束1，必定满足一个请求
//    IloExpr c1(env);
//    for (int i = 0; i < num_requests; ++i)
//        for (int j = 0; j < num_paths; ++j)
//            c1 += satisfy[i][j];
//    model.add(c1 >= 1);
//    c1.end();
//
//    // 约束2，只能选择一个路径
//    IloExpr c2(env);
//    for (int i = 0; i < num_requests; ++i) {
//        for (int j = 0; j < num_paths; ++j) {
//            c2 += satisfy[i][j];
//        }
//        model.add(c2 <= 1);
//        c2.end();
//    }
//
//    // 约束3，资源限制
//    IloExpr c3(env);
//    for (int k = 0; k < num_edges; ++k) {
//        for (int i = 0; i < num_requests; ++i) {
//            for (int j = 0; j < num_paths; ++j) {
//                c3 += satisfy[i][j] * have_edges[i][j][k] * received_rsrc[i];
//            }
//        }
//        model.add(c3 <= edge_rsrc_nums[k]);
//    }
//    c3.end();
//    IloExpr c4(env);
//    for (int k = 0; k < num_nodes; ++k) {
//        for (int i = 0; i < num_requests; ++i) {
//            for (int j = 0; j < num_paths; ++j) {
//                c4 += satisfy[i][j] * have_nodes[i][j][k] * received_rsrc[i];
//            }
//        }
//        model.add(c4 <= node_rsrc_nums[k]);
//    }
//
//    IloCplex cplex(model);//创建求解对象
//    cplex.setParam(IloCplex::TiLim, 0.2);
//    cplex.extract(model);//抽取模型
//    if (cplex.solve()) {
//        cplex.getStatus();//获取当前解的状态，Infeasible\ Feasible\ Optimal
//        //cplex.getObjValue();获取目标函数值，该方法无需指明对象，默认获取的是IloMinimize/IloMaximize的值
//        //可以通过env.out()输出，该方法等同于iostream中的std::cout
//        env.out() << cplex.getObjValue() << "\n";
//        //获取决策变量
//    }
//    cplex.end();//释放对象
//    model.end();
//    env.end();
//
//    return {};
//}

vector<RouteProject*> NetManager::calculate_new_routings(RsrcManager* tmp_rsrc) const {
    vector<RouteProject*> new_route_projects;
    switch (ROUTE_STRTG) {
        case 0: {
            new_route_projects = greedy_routing_projects(tmp_rsrc);
            break;
        }
        case 1: {
            new_route_projects = heuristic_routing_projects(tmp_rsrc);
            break;
        }
//        case 2: {
//            new_route_projects = optimal_routing_projects(tmp_rsrc);
//            break;
//        }
        default: {
            throw logic_error("Unknown ROUTE_STRTG");
        }
    }
    return new_route_projects;
}

void NetManager::schedule_new_routings() {
    //  - New Routings
    RsrcManager* tmp_rsrc;
    switch (RSRC_MANAGE) {
        case 0: {
            tmp_rsrc = new BasicRsrcManager((BasicRsrcManager*)net_rsrc);
            break;
        }
        case 1: {
            tmp_rsrc = new HsRsrcManager((HsRsrcManager*)net_rsrc);
            break;
        }
        default: {
            throw logic_error("Unknown RSRC_MANAGE");
        }
    }
    vector<RouteProject*> new_route_projects = calculate_new_routings(tmp_rsrc);
    switch (RSRC_MANAGE) {
        case 0: {
            delete (BasicRsrcManager*)tmp_rsrc;
            break;
        }
        case 1: {
            delete (HsRsrcManager*)tmp_rsrc;
            break;
        }
        default: {
            throw logic_error("Unknown RSRC_MANAGE");
        }
    }
    for (auto new_route_proj:new_route_projects) {
        reserve_resource(new_route_proj);
        new_route_proj->set_start_time();
        UserRequest* request = new_route_proj->get_request();
        int request_id = request->get_request_id();
        if (waiting_requests.find(request_id) == waiting_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " is not waiting");
        }
        waiting_requests.erase(request_id);
        if (processing_requests.find(request_id) != processing_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " has been processing");
        }
        processing_requests[request_id] = request;
        route_manager->add_new_routing(new_route_proj);
    }
}

void NetManager::static_schedule_new_routings() {
    RsrcManager* tmp_rsrc;
    switch (RSRC_MANAGE) {
        case 0: {
            tmp_rsrc = new BasicRsrcManager((BasicRsrcManager*)net_rsrc);
            break;
        }
        case 1: {
            tmp_rsrc = new HsRsrcManager((HsRsrcManager*)net_rsrc);
            break;
        }
        default: {
            tmp_rsrc = new BasicRsrcManager((BasicRsrcManager*)net_rsrc);
            break;
        }
    }
    vector<RouteProject*> new_route_projects = heuristic_routing_projects(tmp_rsrc);
    switch (RSRC_MANAGE) {
        case 0: {
            delete (BasicRsrcManager*)tmp_rsrc;
            break;
        }
        case 1: {
            delete (HsRsrcManager*)tmp_rsrc;
            break;
        }
        default: {
            delete (BasicRsrcManager*)tmp_rsrc;
            break;
        }
    }
    for (auto new_route_proj:new_route_projects) {
        reserve_resource(new_route_proj);
        new_route_proj->set_start_time();
        UserRequest* request = new_route_proj->get_request();
        int request_id = request->get_request_id();
        if (waiting_requests.find(request_id) == waiting_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " is not waiting");
        }
        waiting_requests.erase(request_id); // similar operation to 'waiting_requests_ordered' has already done in
        // NetManager::static_schedule_new_routings
        if (processing_requests.find(request_id) != processing_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " has been processing");
        }
        processing_requests[request_id] = request;
        route_manager->add_new_routing(new_route_proj);
    }
}


void NetManager::refresh_routing_state(int time) {
    if (route_manager == nullptr) {
        cout << "No Route Manager" << endl;
        return;
    }
    switch (RSRC_MANAGE) {
        case 0: {
            route_manager->refresh_routing_state(time);
            break;
        }
        case 1: {
            route_manager->refresh_routing_state(time, (HsRsrcManager*)net_rsrc);
            break;
        }
        default: {
            throw logic_error("Unknown RSRC_MANAGE");
        }
    }
}

int NetManager::static_check_success_routing(const std::string &runtime_filepath, double run_time) {
    //  - Finish Routings
    ofstream runtime_file;
    runtime_file.open(runtime_filepath, ios::app);
    if (!runtime_file.is_open()) {
        cout << "Cannot Open File " << runtime_filepath << endl;
        return false;
    }
    //  #  req_id  pair_id  cxn_id  fide_th  fide_cxn  task_cpl_time
    runtime_file << setiosflags(ios::fixed) << setprecision(6);
    map<int, UserConnection*> new_req_cxn = route_manager->static_check_success_routing(net_rsrc);//只有这里和非静态的不同
//    if(new_req_cxn.size() == 0){
//        runtime_file << "Fail to "
//    }
    for (auto it_new_cxn: new_req_cxn) {
        int request_id = it_new_cxn.first;
        UserConnection* new_cxn = it_new_cxn.second;
        double task_completion_time = get_time_second(new_cxn->get_created_time(),
                                                      user_requests[request_id]->get_request_time());
        runtime_file << "TaskCpl" << "\t";
        runtime_file << request_id << "\t";
        runtime_file << user_requests[request_id]->get_pair_id() << "\t";
        runtime_file << new_cxn->get_connection_id() << "\t";
        runtime_file << user_requests[request_id]->get_fide_th() << "\t";
        runtime_file << new_cxn->get_fidelity() << "\t";
        runtime_file << task_completion_time << endl;
        if (user_connections.find(request_id) != user_connections.end()) {
            throw logic_error("Request " + to_string(request_id) + " already has connection");
        }
        user_connections[request_id] = new_cxn;
        UserRequest* request = user_requests[request_id];
        if (processing_requests.find(request_id) == processing_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " is not processing");
        }
        processing_requests.erase(request_id);
        if (serving_requests.find(request_id) != serving_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " has been serving");
        }
        serving_requests[request_id] = request;
        user_requests[request_id]->set_created();
    }
    runtime_file.close();
    return (int)new_req_cxn.size();
}

int NetManager::check_success_routing(const string& runtime_filepath) {
    //  - Finish Routings
    ofstream runtime_file;
    runtime_file.open(runtime_filepath, ios::app);
    if (!runtime_file.is_open()) {
        cout << "Cannot Open File " << runtime_filepath << endl;
        return false;
    }
    //  #  req_id  pair_id  cxn_id  fide_th  fide_cxn  task_cpl_time
    runtime_file << setiosflags(ios::fixed) << setprecision(6);
    map<int, UserConnection*> new_req_cxn = route_manager->check_success_routing(net_rsrc);
    for (auto it_new_cxn: new_req_cxn) {
        int request_id = it_new_cxn.first;
        UserConnection* new_cxn = it_new_cxn.second;
        double task_completion_time = get_time_second(new_cxn->get_created_time(),
                                                      user_requests[request_id]->get_request_time());
        runtime_file << "TaskCpl" << "\t";
        runtime_file << request_id << "\t";
        runtime_file << user_requests[request_id]->get_pair_id() << "\t";
        runtime_file << new_cxn->get_connection_id() << "\t";
        runtime_file << user_requests[request_id]->get_fide_th() << "\t";
        runtime_file << new_cxn->get_fidelity() << "\t";
        runtime_file << task_completion_time << endl;
        if (user_connections.find(request_id) != user_connections.end()) {
            throw logic_error("Request " + to_string(request_id) + " already has connection");
        }
        user_connections[request_id] = new_cxn;
        UserRequest* request = user_requests[request_id];
        if (processing_requests.find(request_id) == processing_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " is not processing");
        }
        processing_requests.erase(request_id);
        if (serving_requests.find(request_id) != serving_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " has been serving");
        }
        serving_requests[request_id] = request;
        user_requests[request_id]->set_created();
        req_delay[request_id] = get_time_interval(user_requests[request_id]->get_satisfy_time(),
                                                  user_requests[request_id]->get_request_time());
    }
    runtime_file.close();
    return (int)new_req_cxn.size();
}

int NetManager::check_killed_routing(const string& runtime_filepath) {
    vector<int> failed_request = route_manager->check_killed_routing(net_rsrc);
    for (auto request_id:failed_request) {
        UserRequest* request = user_requests[request_id];
        if (processing_requests.find(request_id) == processing_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " is not processing");
        }
        processing_requests.erase(request_id);
        if (waiting_requests.find(request_id) != waiting_requests.end()) {
            throw logic_error("Request " + to_string(request_id) + " is waiting");
        }
        waiting_requests[request_id] = request;
    }
    return (int)failed_request.size();
}

int NetManager::finish_user_connection(int time) {
    //  - Finish Connections
    vector<int> finished_request;
    for (auto& it_user_cxn:user_connections) {
        int request_id = it_user_cxn.first;
        UserConnection* user_cxn = it_user_cxn.second;
        user_cxn->add_age(time);
        if (user_cxn->is_expired()) {
            int route_id = user_cxn->get_route_id();
            user_cxn->finish_connection();
            delete user_cxn;
            finished_request.push_back(request_id);
            int s_id = serving_requests[request_id]->get_s_node_id();
            int d_id = serving_requests[request_id]->get_d_node_id();
            if (RSRC_MANAGE == 0 || RSRC_MANAGE == 1) {
                net_rsrc->release_node_memory(s_id, 1, route_id);
                net_rsrc->release_node_memory(d_id, 1, route_id);
            }
            if (serving_requests.find(request_id) == serving_requests.end()) {
                throw logic_error("Request " + to_string(request_id) + " is not serving");
            }
            serving_requests.erase(request_id);
            user_requests[request_id]->set_closed();
//            if (user_requests[request_id]->has_closed()) {
//                delete user_requests[request_id];
//                user_requests.erase(request_id);
//            }
        }
    }
    for (auto request_id:finished_request) {
        user_connections.erase(request_id);
    }
    return (int)finished_request.size();
}

double NetManager::calculate_obj(UserRequest* request, RouteProject* route) {
    int wait_time = get_time_interval(get_current_time(), request->get_request_time());
    double e_slot = route->get_expect_slots();
    return wait_time / e_slot;
}

int NetManager::get_leftover_routes() {
    return route_manager -> get_leftover_routes();
}
