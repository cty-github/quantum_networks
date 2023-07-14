//
// Created by TianyaoChu on 2023/5/17.
//

#ifndef QUANTUM_NETWORKS_TEST_PARAMS_H
#define QUANTUM_NETWORKS_TEST_PARAMS_H

// net_topo parameters
#define PTNSRC_NUM 10
#define BSM_NUM 10
#define USER_NUM 20
#define REPEATER_NUM 30
#define NET_SIZE 10000
#define ALPHA 0.4
#define BETA 0.9    // \beta*e^{-l/(\alpha*L)}
#define MEM_LOW 10
#define MEM_UP 14
#define CAP_LOW 3
#define CAP_UP 7

// device parameters
#define PTN_DECAY_RATE 0.002
#define PTN_FIDE 0.99
#define BSM_SUCCESS_RATE 1.0
#define BSM_Z_FIDE 0.999
#define BSM_X_FIDE 0.999

// working parameters
#define CANDIDATE_NUM 5
#define RUN_TIME 5
#define IPS 10000
#define SAMPLE_INT 1000

// realization methods
#define ROUTE_STRTG 0   // route strategy type
#define RSRC_MANAGE 0   // net rsrc manager type

// heuristic optimization (ROUTE_STRTG 1)
#define NUM_SELECTED_REQUESTS 15
#define NUM_REPEATS 10
#define NUM_TRIES 10000

// request parameters
#define SD_RATIO 1.0    // determine serve sd pair
#define SD_FIDE_UP 0.99
#define SD_FIDE_LOW 0.95
#define TIME_PROB 0.001
#define SD_PROB 0.1     // determine request sd pair
#define REQ_RATE 0.2    // determine request num

// entangle parameters
#define LIFESPAN 10000

#endif //QUANTUM_NETWORKS_TEST_PARAMS_H
