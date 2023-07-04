//
// Created by TianyaoChu on 2023/5/7.
//

#ifndef QUANTUM_NETWORKS_NET_RESOURCE_H
#define QUANTUM_NETWORKS_NET_RESOURCE_H

#include "net_topology/net_topology.h"
#include <vector>
#include <queue>

enum RsrcType {UserMemory, RptrMemory, Channel};
enum RsrcStatus {Free, Busy, SoftFree, Reused};

class RsrcTracker { // tracking the utilization of resources in some node or edge
private:
    RsrcType rsrc_type;
    int ne_id;  // node/edge_id
    int rsrc_num;
    vector<RsrcStatus> rsrc_status_list;
    vector<int> rsrc_serve_route;   // list for route first use the rsrc
    vector<int> rsrc_reuse_route;   // list for route reuse the rsrc
//    vector<vector<int>> serve_route_list; // list for routing assigned the rsrc
public:
    RsrcTracker(RsrcType rsrc_type, int ne_id, int rsrc_num);
    ~RsrcTracker();
    RsrcType get_rsrc_type() const;
    int get_ne_id() const;
    int get_rsrc_num() const;
    bool is_free(int rsrc_id) const;
    bool is_busy(int rsrc_id) const;
    int get_rsrc_status(int rsrc_id) const;
    int get_serve_route(int rsrc_id) const;
    int get_reuse_route(int rsrc_id) const;
    int get_free_num() const;
    int get_route_total_rsrc_num(int route_id) const;
    queue<int> get_free_queue() const;
    queue<int> get_busy_queue_route(int route_id) const;
    queue<int> get_soft_free_queue() const;
    queue<int> get_soft_free_queue_route(int route_id) const;
    queue<int> get_reused_queue_route(int route_id) const;
    queue<int> get_reusing_queue_route(int route_id) const;
//    queue<int> get_serve_queue_route(int route_id) const;
    queue<int> get_soft_released_queue_route(int route_id) const;
    bool reserve_for_route(int route_id, int num);
    vector<int> preempt_for_route(int route_id, int num);
    bool hard_release_route(int route_id, int num);
    bool soft_release_route(int route_id, int num);
    bool release_route_all(int route_id);
};

class RsrcManager {
protected:
    NetTopology* net_topo;
public:
    explicit RsrcManager(NetTopology* net_topo);
    ~RsrcManager();
    virtual int get_free_node_memory(int node_id) const = 0;
    virtual int get_free_edge_channel(int edge_id) const = 0;
    virtual bool reserve_node_memory(int node_id, int num, int route_id) = 0;
    virtual bool reserve_edge_channel(int edge_id, int num, int route_id) = 0;
    virtual bool release_node_memory(int node_id, int num, int route_id) = 0;
    virtual bool release_edge_channel(int edge_id, int num, int route_id) = 0;
    virtual bool check_link_resource(int s_id, int d_id, int num) const;
    virtual int max_link_resource(int s_id, int d_id) const;
    virtual bool reserve_link_resource(int s_id, int d_id, int num, int route_id);
    virtual bool release_link_resource(int s_id, int d_id, int num, int route_id);
};

class BasicRsrcManager: public RsrcManager {
private:
    map<int, int> free_node_memory;
    map<int, int> free_edge_channel;
public:
    explicit BasicRsrcManager(NetTopology* net_topo);
    explicit BasicRsrcManager(BasicRsrcManager* rsrc_mgr);
    ~BasicRsrcManager();
    int get_free_node_memory(int node_id) const override;
    int get_free_edge_channel(int edge_id) const override;
    bool reserve_node_memory(int node_id, int num, int route_id) override;
    bool reserve_edge_channel(int edge_id, int num, int route_id) override;
    bool release_node_memory(int node_id, int num, int route_id) override;
    bool release_edge_channel(int edge_id, int num, int route_id) override;
};

class HsRsrcManager: public RsrcManager {
private:
    map<int, RsrcTracker*> node_memory_tracker;  // {node_id, memories_in_node}
    map<int, RsrcTracker*> edge_channel_tracker; // {edge_id, channels_on_edge}
public:
    explicit HsRsrcManager(NetTopology* net_topo);
    explicit HsRsrcManager(HsRsrcManager* rsrc_mgr);
    ~HsRsrcManager();
    int get_free_node_memory(int node_id) const override;
    int get_free_edge_channel(int edge_id) const override;
    bool reserve_node_memory(int node_id, int num, int route_id) override;
    bool reserve_edge_channel(int edge_id, int num, int route_id) override;
    vector<int> preempt_node_memory(int node_id, int num, int route_id);
    vector<int> preempt_edge_capacity(int edge_id, int num, int route_id);
    bool release_node_memory(int node_id, int num, int route_id) override;
    bool release_edge_channel(int edge_id, int num, int route_id) override;
    bool soft_release_node_memory(int node_id, int num, int route_id);
    bool soft_release_edge_channel(int edge_id, int num, int route_id);
};

#endif //QUANTUM_NETWORKS_NET_RESOURCE_H
