//
// Created by TianyaoChu on 2023/3/28.
//

#include "rand.h"

random_device rand_dev;
default_random_engine rand_eng(rand_dev());
