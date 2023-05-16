//
// Created by rozhin on 15.05.23.
//

#ifndef MAPREDUCE_MAP_BASE_H
#define MAPREDUCE_MAP_BASE_H

#include <string>

namespace mapreduce {

class map_task_base {
public:
    typedef std::string key_type;
    typedef size_t      value_type;

public:
    map_task_base();
    map_task_base(...);
    virtual ~map_task_base();

public:
    virtual void operator()(const key_type& key, const value_type& value);

};

} // mapreduce

#endif //MAPREDUCE_MAP_BASE_H
