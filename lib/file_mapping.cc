//
// Created by rozhin on 15.05.23.
//

#include "job/file_mapping.h"

namespace mapreduce {

file_mapping_handler::file_mapping_handler(int32_t fd, size_t fsize, char *data)
        :
        f_desc_(fd),
        f_size_(fsize),
        data_(data) {}

}