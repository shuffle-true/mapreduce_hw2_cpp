//
// Created by rozhin on 15.05.23.
//

#ifndef MAPREDUCE_JOB_H
#define MAPREDUCE_JOB_H

#include "../utils/job_context.h"
#include "../utils/macro.h"
#include "file_mapping.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>


namespace mapreduce {

class job {
public:
    job(JobContext& context);

public:
    enum class status { ok, bad };

public:
    void start();

private:
    /*
     * Выполняет предварительную обработку входных файлов.
     * 1. Мержит между собой по порядку, переданному в filenames_
     * 2. Бьет на сплиты по N_BYTES_SPLIT_SIZE байт
     */
    void split_file_routine();

private:
    JobContext ctx;
    std::unique_ptr<file_mapping_handler> file_mapping_handler_;
};

} // mapreduce

#endif //MAPREDUCE_JOB_H
