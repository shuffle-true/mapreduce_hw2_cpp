//
// Created by rozhin on 18.05.23.
//

#ifndef MAPREDUCE_MAPREDUCE_CONTEXT_H
#define MAPREDUCE_MAPREDUCE_CONTEXT_H

#include <vector>
#include <string_view>
#include <map>

#include "utils.h"

namespace mapreduce {

class MapReduceContext {
public:
    MapReduceContext();

public:
    map_vec mapper_results;
};

} // mapreduce

#endif //MAPREDUCE_MAPREDUCE_CONTEXT_H
