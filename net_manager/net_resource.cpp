//
// Created by TianyaoChu on 2023/5/7.
//

#include "net_resource.h"
#include "utils/tool.h"
#include <iostream>
#include <algorithm>

RsrcTracker::RsrcTracker(RsrcType rsrc_type, int ne_id, int rsrc_num):
rsrc_type(rsrc_type), ne_id(ne_id), rsrc_num(rsrc_num),
rsrc_status_list(rsrc_num, Free), rsrc_serve_route(rsrc_num, -1), rsrc_reuse_route(rsrc_num, -1) {}

RsrcTracker::~RsrcTracker() = default;

RsrcType RsrcTracker::get_rsrc_type() const {
    return rsrc_type;
}

int RsrcTracker::get_ne_id() const {
    return ne_id;
}

int RsrcTracker::get_rsrc_num() const {
    return rsrc_num;
}

bool RsrcTracker::is_free(int rsrc_id) const {
    return rsrc_status_list[rsrc_id]==Free;
}

bool RsrcTracker::is_busy(int rsrc_id) const {
    return rsrc_status_list[rsrc_id]==Busy;
}

int RsrcTracker::get_rsrc_status(int rsrc_id) const {
    return rsrc_status_list[rsrc_id];
}

int RsrcTracker::get_serve_route(int rsrc_id) const {
    return rsrc_serve_route[rsrc_id];
}

int RsrcTracker::get_reuse_route(int rsrc_id) const {
    return rsrc_reuse_route[rsrc_id];
}

int RsrcTracker::get_free_num() const {
    int free_num = 0;
    for (int i = 0; i < rsrc_num; i++) {
        if (rsrc_status_list[i] == Free) {
            free_num++;
        }
    }
    return free_num;
}

int RsrcTracker::get_route_total_rsrc_num(int route_id) const {
    int route_rsrc_num = 0;
    for (int i = 0; i < rsrc_num; i++) {
        if (rsrc_serve_route[i] == route_id || rsrc_reuse_route[i] == route_id) {
            route_rsrc_num++;
        }
    }
    return  route_rsrc_num;
}

queue<int> RsrcTracker::get_free_queue() const {
    queue<int> free_queue;
    for (int i = 0; i < rsrc_num; i++) {
        if (rsrc_status_list[i] == Free) {
            free_queue.push(i);
        }
    }
    return free_queue;
}

queue<int> RsrcTracker::get_busy_queue_route(int route_id) const {
    queue<int> busy_queue_route;
    for (int i = 0; i < rsrc_num; i++) {
        if (rsrc_status_list[i] == Busy && rsrc_serve_route[i] == route_id) {
            busy_queue_route.push(i);
        }
    }
    return busy_queue_route;
}

queue<int> RsrcTracker::get_soft_free_queue() const {
    queue<int> soft_free_queue;
    for (int i = 0; i < rsrc_num; i++) {
        if (rsrc_status_list[i] == SoftFree) {
            soft_free_queue.push(i);
        }
    }
    return soft_free_queue;
}

queue<int> RsrcTracker::get_soft_free_queue_route(int route_id) const {
    queue<int> soft_free_queue_route;
    for (int i = 0; i < rsrc_num; i++) {
        if (rsrc_status_list[i] == SoftFree && rsrc_serve_route[i] == route_id) {
            soft_free_queue_route.push(i);
        }
    }
    return soft_free_queue_route;
}

queue<int> RsrcTracker::get_reused_queue_route(int route_id) const {  // queue of route id's reused hard rsrc
    queue<int> reused_queue_route;
    for (int i = 0; i < rsrc_num; i++) {
        if (rsrc_status_list[i] == Reused && rsrc_serve_route[i] == route_id) {
            reused_queue_route.push(i);
        }
    }
    return reused_queue_route;
}

queue<int> RsrcTracker::get_reusing_queue_route(int route_id) const {   // queue of route id's soft rsrc
    queue<int> reusing_queue_route;
    for (int i = 0; i < rsrc_num; i++) {
        if (rsrc_status_list[i] == Reused && rsrc_reuse_route[i] == route_id) {
            reusing_queue_route.push(i);
        }
    }
    return reusing_queue_route;
}

queue<int> RsrcTracker::get_soft_released_queue_route(int route_id) const {
    queue<int> soft_released_queue_route;
    for (int i = 0; i < rsrc_num; i++) {
        if ((rsrc_status_list[i] == SoftFree || rsrc_status_list[i] == Reused) && rsrc_serve_route[i] == route_id) {
            soft_released_queue_route.push(i);
        }
    }
    return soft_released_queue_route;
}

bool RsrcTracker::reserve_for_route(int route_id, int num) {
    queue<int> free_queue = get_free_queue();
    queue<int> soft_free_queue = get_soft_free_queue();
    if (free_queue.size() + soft_free_queue.size() < num) {
        throw logic_error("No Enough Free Resource");
    }
    while (num--) {
        if (!free_queue.empty()) {
            int rsrc_id = free_queue.front();
            rsrc_status_list[rsrc_id] = Busy;
            rsrc_serve_route[rsrc_id] = route_id;
            free_queue.pop();
        } else {
            int rsrc_id = soft_free_queue.front();
            rsrc_status_list[rsrc_id] = Reused;
            rsrc_reuse_route[rsrc_id] = route_id;
            soft_free_queue.pop();
        }
    }
    return true;
}

vector<int> RsrcTracker::preempt_for_route(int route_id, int num) {
    queue<int> soft_free_queue = get_soft_free_queue_route(route_id);
    queue<int> reused_queue = get_reused_queue_route(route_id);
    if (soft_free_queue.size() + reused_queue.size() < num) {
        throw logic_error("Preempt Exceed Occupied Resource");
    }
    queue<int> preempted_route;
    while (num--) {
        if (!soft_free_queue.empty()) {
            int rsrc_id = soft_free_queue.front();
            rsrc_status_list[rsrc_id] = Busy;
            soft_free_queue.pop();
        } else {
            int rsrc_id = reused_queue.front();
            preempted_route.push(rsrc_reuse_route[rsrc_id]);
            rsrc_status_list[rsrc_id] = Busy;
            rsrc_reuse_route[rsrc_id] = -1;
            reused_queue.pop();
        }
    }
    vector<int> stop_route;
    while (!preempted_route.empty()) {
        int preempted_route_id = preempted_route.front();
        int rest_rsrc_num = get_route_total_rsrc_num(preempted_route_id);
        if (rsrc_type == Channel || rsrc_type == UserMemory) {
            if (rest_rsrc_num <= 0) {
                stop_route.push_back(preempted_route_id);
            }
        } else if (rsrc_type == RptrMemory) {
            if (rest_rsrc_num <=1) {
                stop_route.push_back(preempted_route_id);
            }
        }
        preempted_route.pop();
    }
    return stop_route;
}

bool RsrcTracker::hard_release_route(int route_id, int num) {
    queue<int> reusing_queue = get_reusing_queue_route(route_id);
    queue<int> reused_queue = get_reused_queue_route(route_id);
    queue<int> soft_released_queue = get_soft_free_queue_route(route_id);
    queue<int> busy_queue = get_busy_queue_route(route_id);
    if (reusing_queue.size() + reused_queue.size() + soft_released_queue.size() + busy_queue.size() < num) {
        throw logic_error("Hard Release Exceed Occupied Resource");
    }
    while (num--) {
        if (!reusing_queue.empty()) {
            int rsrc_id = reusing_queue.front();
            rsrc_status_list[rsrc_id] = SoftFree;
            rsrc_reuse_route[rsrc_id] = -1;
            reusing_queue.pop();
        } else if (!reused_queue.empty()) {
            int rsrc_id = reused_queue.front();
            rsrc_status_list[rsrc_id] = Busy;
            rsrc_serve_route[rsrc_id] = rsrc_reuse_route[rsrc_id];
            rsrc_reuse_route[rsrc_id] = -1;
            reused_queue.pop();
        } else if (!soft_released_queue.empty()) {
            int rsrc_id = soft_released_queue.front();
            rsrc_status_list[rsrc_id] = Free;
            rsrc_serve_route[rsrc_id] = -1;
            soft_released_queue.pop();
        } else {
            int rsrc_id = busy_queue.front();
            rsrc_status_list[rsrc_id] = Free;
            rsrc_serve_route[rsrc_id] = -1;
            busy_queue.pop();
        }
    }
    return true;
}

bool RsrcTracker::soft_release_route(int route_id, int num) {
    // soft release hard resource of route id
    queue<int> busy_queue = get_busy_queue_route(route_id);
    if (busy_queue.size() < num) {
        throw logic_error("Soft Release Exceed Hard Resource");
    }
    while (num--) {
        int rsrc_id = busy_queue.front();
        rsrc_status_list[rsrc_id] = SoftFree;
        busy_queue.pop();
    }
    return true;
}

bool RsrcTracker::release_route_all(int route_id) {
    for (int i = 0; i < rsrc_num; i++) {
        if (rsrc_serve_route[i] == route_id) {
            if (rsrc_status_list[i] == Busy || rsrc_status_list[i] == SoftFree) {
                rsrc_status_list[i] = Free;
                rsrc_serve_route[i] = -1;
            } else if (rsrc_status_list[i] == Reused) {
                rsrc_status_list[i] = Busy;
                rsrc_serve_route[i] = rsrc_reuse_route[i];
                rsrc_reuse_route[i] = -1;
            }
        } else if (rsrc_reuse_route[i] == route_id) {
            rsrc_status_list[i] = SoftFree;
            rsrc_reuse_route[i] = -1;
        }
    }
    return true;
}

RsrcManager::RsrcManager(NetTopology* net_topo): net_topo(net_topo) {}

RsrcManager::~RsrcManager() = default;

bool RsrcManager::check_link_resource(int s_id, int d_id, int num) const {
    if (get_free_node_memory(s_id) < num) {
        cout << "No Enough UserMemory in Node " << s_id << endl;
        return false;
    }
    if (get_free_node_memory(d_id) < num) {
        cout << "No Enough UserMemory in Node " << d_id << endl;
        return false;
    }
    int edge_id = net_topo->get_edge(s_id, d_id)->get_edge_id();
    if (get_free_edge_channel(edge_id) < num) {
        cout << "No Enough Capacity in Edge " << edge_id << endl;
        return false;
    }
    return true;
}

int RsrcManager::max_link_resource(int s_id, int d_id) const {
    int edge_id = net_topo->get_edge(s_id, d_id)->get_edge_id();
    int max_num = get_free_edge_channel(edge_id);
    if (net_topo->get_node(s_id)->is_user()) {
        if (max_num > get_free_node_memory(s_id)) {
            max_num = get_free_node_memory(s_id);
        }
    } else {
        if (max_num > get_free_node_memory(s_id)/2) {
            max_num = get_free_node_memory(s_id)/2;
        }
    }
    if (net_topo->get_node(d_id)->is_user()) {
        if (max_num > get_free_node_memory(d_id)) {
            max_num = get_free_node_memory(d_id);
        }
    } else {
        if (max_num > get_free_node_memory(d_id)/2) {
            max_num = get_free_node_memory(d_id)/2;
        }
    }
    return max_num;
}

bool RsrcManager::reserve_link_resource(int s_id, int d_id, int num, int route_id) {
    if (!reserve_node_memory(s_id, num, route_id)) {
        return false;
    }
    if (!reserve_node_memory(d_id, num, route_id)) {
        release_node_memory(s_id, num, route_id);
        return false;
    }
    int edge_id = net_topo->get_edge(s_id, d_id)->get_edge_id();
    if (!reserve_edge_channel(edge_id, num, route_id)) {
        release_node_memory(s_id, num, route_id);
        release_node_memory(d_id, num, route_id);
        return false;
    }
    return true;
}

bool RsrcManager::release_link_resource(int s_id, int d_id, int num, int route_id) {
    if (!release_node_memory(s_id, num, route_id)) {
        return false;
    }
    if (!release_node_memory(d_id, num, route_id)) {
        reserve_node_memory(s_id, num, route_id);
        return false;
    }
    int edge_id = net_topo->get_edge(s_id, d_id)->get_edge_id();
    if (!release_edge_channel(edge_id, num, route_id)) {
        reserve_node_memory(s_id, num, route_id);
        reserve_node_memory(d_id, num, route_id);
        return false;
    }
    return true;
}

BasicRsrcManager::BasicRsrcManager(NetTopology* net_topo): RsrcManager(net_topo) {
    for (int i = 0; i < net_topo->get_node_num(); i++) {
        free_node_memory[i] = net_topo->get_node(i)->get_memory_size();
    }
    for (int i = 0; i < net_topo->get_edge_num(); i++) {
        free_edge_channel[i] = net_topo->get_edge(i)->get_channel_capacity();
    }
}

BasicRsrcManager::BasicRsrcManager(BasicRsrcManager* rsrc_mgr): RsrcManager(rsrc_mgr->net_topo),
free_node_memory(rsrc_mgr->free_node_memory), free_edge_channel(rsrc_mgr->free_edge_channel) {}

BasicRsrcManager::~BasicRsrcManager() = default;

int BasicRsrcManager::get_free_node_memory(int node_id) const {
    if (free_node_memory.find(node_id) == free_node_memory.end()) {
        throw logic_error("No Node " + to_string(node_id));
    }
    return free_node_memory.find(node_id)->second;
}

int BasicRsrcManager::get_free_edge_channel(int edge_id) const {
    if (free_edge_channel.find(edge_id) == free_edge_channel.end()) {
        throw logic_error("No Edge " + to_string(edge_id));
    }
    return free_edge_channel.find(edge_id)->second;
}

bool BasicRsrcManager::reserve_node_memory(int node_id, int num, int route_id) {
    if (get_free_node_memory(node_id) < num) {
        cout << "No Enough UserMemory in Node " << node_id << endl;
        return false;
    }
    free_node_memory[node_id] -= num;
//    cout << "Reserve " << num << " UserMemory in Node " << node_id << endl;
    return true;
}

bool BasicRsrcManager::reserve_edge_channel(int edge_id, int num, int route_id) {
    if (get_free_edge_channel(edge_id) < num) {
        cout << "No Enough Capacity in Edge " << edge_id << endl;
        return false;
    }
    free_edge_channel[edge_id] -= num;
//    cout << "Reserve " << num << " Capacity in Edge " << edge_id << endl;
    return true;
}

bool BasicRsrcManager::release_node_memory(int node_id, int num, int route_id) {
    if (get_free_node_memory(node_id)+num > net_topo->get_node(node_id)->get_memory_size()) {
        cout << "Exceed UserMemory of Node " << node_id << endl;
        return false;
    }
    free_node_memory[node_id] += num;
//    cout << "Release " << num << " UserMemory in Node " << node_id << endl;
    return true;
}

bool BasicRsrcManager::release_edge_channel(int edge_id, int num, int route_id) {
    if (get_free_edge_channel(edge_id) + num > net_topo->get_edge(edge_id)->get_channel_capacity()) {
        cout << "Exceed Capacity of Edge " << edge_id << endl;
        return false;
    }
    free_edge_channel[edge_id] += num;
//    cout << "Release " << num << " Capacity in Edge " << edge_id << endl;
    return true;
}

HsRsrcManager::HsRsrcManager(NetTopology* net_topo): RsrcManager(net_topo) {
    for (int i = 0; i < net_topo->get_node_num(); i++) {
        node_memory_tracker[i] = new RsrcTracker(UserMemory, i,
                                                 net_topo->get_node(i)->get_memory_size());
    }
    for (int i = 0; i < net_topo->get_edge_num(); i++) {
        edge_channel_tracker[i] = new RsrcTracker(Channel, i,
                                                  net_topo->get_edge(i)->get_channel_capacity());
    }
}

HsRsrcManager::HsRsrcManager(HsRsrcManager* rsrc_mgr): RsrcManager(rsrc_mgr->net_topo),
node_memory_tracker(rsrc_mgr->node_memory_tracker), edge_channel_tracker(rsrc_mgr->edge_channel_tracker) {}

HsRsrcManager::~HsRsrcManager() = default;

int HsRsrcManager::get_free_node_memory(int node_id) const {
    if (node_memory_tracker.find(node_id) == node_memory_tracker.end()) {
        throw logic_error("No Node " + to_string(node_id));
    }
    return node_memory_tracker.find(node_id)->second->get_free_num();
}

int HsRsrcManager::get_free_edge_channel(int edge_id) const {
    if (edge_channel_tracker.find(edge_id) == edge_channel_tracker.end()) {
        throw logic_error("No Edge " + to_string(edge_id));
    }
    return edge_channel_tracker.find(edge_id)->second->get_free_num();
}

bool HsRsrcManager::reserve_node_memory(int node_id, int num, int route_id) {
    if (node_memory_tracker.find(node_id) == node_memory_tracker.end()) {
        throw logic_error("No Node " + to_string(node_id));
    }
    return node_memory_tracker[node_id]->reserve_for_route(route_id, num);
}

bool HsRsrcManager::reserve_edge_channel(int edge_id, int num, int route_id) {
    if (edge_channel_tracker.find(edge_id) == edge_channel_tracker.end()) {
        throw logic_error("No Edge " + to_string(edge_id));
    }
    return edge_channel_tracker[edge_id]->reserve_for_route(route_id, num);
}

vector<int> HsRsrcManager::preempt_node_memory(int node_id, int num, int route_id) {
    if (node_memory_tracker.find(node_id) == node_memory_tracker.end()) {
        throw logic_error("No Node " + to_string(node_id));
    }
    vector<int> stop_route = node_memory_tracker[node_id]->preempt_for_route(route_id, num);
    for (auto stop_id:stop_route) {
        for (auto track:node_memory_tracker) {
            track.second->release_route_all(stop_id);
        }
        for (auto track:edge_channel_tracker) {
            track.second->release_route_all(stop_id);
        }
    }
    return stop_route;
}

vector<int> HsRsrcManager::preempt_edge_capacity(int edge_id, int num, int route_id) {
    if (edge_channel_tracker.find(edge_id) == edge_channel_tracker.end()) {
        throw logic_error("No Edge " + to_string(edge_id));
    }
    vector<int> stop_route = edge_channel_tracker[edge_id]->preempt_for_route(route_id, num);
    for (auto stop_id:stop_route) {
        for (auto track:node_memory_tracker) {
            track.second->release_route_all(stop_id);
        }
        for (auto track:edge_channel_tracker) {
            track.second->release_route_all(stop_id);
        }
    }
    return stop_route;
}

bool HsRsrcManager::release_node_memory(int node_id, int num, int route_id) {
    if (node_memory_tracker.find(node_id) == node_memory_tracker.end()) {
        throw logic_error("No Node " + to_string(node_id));
    }
    return node_memory_tracker[node_id]->hard_release_route(route_id, num);
}

bool HsRsrcManager::release_edge_channel(int edge_id, int num, int route_id) {
    if (edge_channel_tracker.find(edge_id) == edge_channel_tracker.end()) {
        throw logic_error("No Edge " + to_string(edge_id));
    }
    return edge_channel_tracker[edge_id]->hard_release_route(route_id, num);
}

bool HsRsrcManager::soft_release_node_memory(int node_id, int num, int route_id) {
    if (node_memory_tracker.find(node_id) == node_memory_tracker.end()) {
        throw logic_error("No Node " + to_string(node_id));
    }
    return node_memory_tracker[node_id]->soft_release_route(route_id, num);
}

bool HsRsrcManager::soft_release_edge_channel(int edge_id, int num, int route_id) {
    if (edge_channel_tracker.find(edge_id) == edge_channel_tracker.end()) {
        throw logic_error("No Edge " + to_string(edge_id));
    }
    return edge_channel_tracker[edge_id]->soft_release_route(route_id, num);
}
