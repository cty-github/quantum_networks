cmake_minimum_required(VERSION 3.24)
project(quantum_networks)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(quantum_networks
        main.cpp
        qnetwork.cpp qnetwork.h
        utils/rand.cpp utils/rand.h
        utils/tool.cpp utils/tool.h
        net_topology/net_topology.cpp net_topology/net_topology.h
        net_topology/qnode.cpp net_topology/qnode.h
        net_topology/qedge.cpp net_topology/qedge.h
        net_topology/path.cpp net_topology/path.h
        qdevice/qdevice.cpp qdevice/qdevice.h
        qdevice/device_manager.cpp qdevice/device_manager.h
        entangle_level/entangle_level.cpp entangle_level/entangle_level.h
        entangle_level/qelement.cpp entangle_level/qelement.h
        net_manager/net_manager.cpp net_manager/net_manager.h
        net_manager/route_manager.cpp net_manager/route_manager.h
        net_manager/link_manager.cpp net_manager/link_manager.h
        net_manager/user_service.cpp net_manager/user_service.h
        net_manager/net_resource.cpp net_manager/net_resource.h
        algorithms/algorithms.cpp algorithms/algorithms.h
        algorithms/multi_r.cpp
        algorithms/reps.cpp
        algorithms/efirap.cpp utils/timing.cpp utils/timing.h test_params.h)
