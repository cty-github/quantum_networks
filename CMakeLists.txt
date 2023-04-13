cmake_minimum_required(VERSION 3.24)
project(quantum_networks)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(quantum_networks
        main.cpp
        qnetwork.cpp qnetwork.h
        utils/rand.cpp utils/rand.h
        net_topology/net_topology.cpp net_topology/net_topology.h
        net_topology/qnode.cpp net_topology/qnode.h
        net_topology/qedge.cpp net_topology/qedge.h
        qdevice/qdevice.cpp qdevice/qdevice.h
        qdevice/device_manager.cpp qdevice/device_manager.h
        algorithms/multi_r.cpp
        algorithms/reps.cpp
        algorithms/efirap.cpp
        algorithms/algorithms.cpp
        algorithms/algorithms.h
        net_topology/routing.cpp net_topology/routing.h
        qdevice/qelement.cpp qdevice/qelement.h utils/tool.cpp utils/tool.h net_manager/net_manager.cpp net_manager/net_manager.h)
