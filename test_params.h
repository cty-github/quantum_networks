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
#define CAP_UP 14
#define CAP_LOW 10

// device parameters
#define PTN_DECAY_RATE 0.002
#define PTN_FIDE 0.99
#define BSM_SUCCESS_RATE 1.0
#define BSM_Z_FIDE 0.999
#define BSM_X_FIDE 0.999

// working parameters
#define CANDIDATE_NUM 5
#define RUN_TIME 10

// request parameters
#define SD_RATIO 1.0
#define SD_FIDE_UP 0.99
#define SD_FIDE_LOW 0.95
#define TIME_PROB 0.001
#define SD_PROB 0.1
#define REQ_RATE 0.2

// entangle parameters
#define LIFESPAN 10000;

#endif //QUANTUM_NETWORKS_TEST_PARAMS_H
