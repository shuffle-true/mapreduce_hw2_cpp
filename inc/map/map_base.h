//
// Created by rozhin on 15.05.23.
//

#ifndef MAPREDUCE_MAP_BASE_H
#define MAPREDUCE_MAP_BASE_H

#include <string>
#include "../utils/utils.h"

namespace mapreduce {

class MapReduceContext;

class map_task_base {
public:
    map_task_base();
    virtual ~map_task_base();

public:
    virtual void operator()(const char* key, const size_t value, pair_vec& result) = 0;

};

} // mapreduce

#endif //MAPREDUCE_MAP_BASE_H
