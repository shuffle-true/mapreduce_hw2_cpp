//
// Created by rozhin on 15.05.23.
//

#ifndef MAPREDUCE_FILE_MAPPING_H
#define MAPREDUCE_FILE_MAPPING_H

#include <cstdlib>

namespace mapreduce {

class job;

class file_mapping_handler {
public:
    file_mapping_handler(int32_t fd, size_t fsize, char* data);

private:
    int32_t f_desc_;
    size_t f_size_;
    char* data_;

    friend class job;
};

} // mapreduce

#endif //MAPREDUCE_FILE_MAPPING_H
