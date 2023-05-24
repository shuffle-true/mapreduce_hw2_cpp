//
// Created by rozhin on 15.05.23.
//

#ifndef MAPREDUCE_JOB_H
#define MAPREDUCE_JOB_H

#include "../utils/job_context.h"
#include "../utils/mapreduce_context.h"
#include "../utils/macro.h"
#include "../utils/utils.h"
#include "../concurrency/threadpool.h"
#include "file_mapping.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>
#include <cassert>

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

    /*
     * Запуск мапперов на полученных сплитах данных
     */
    void run_map_task();

    /*
     * Изменяет tensor_t объект. На выходе у каждого маппера num_reducers контейнеров,
     * содержащих результат работы маппера
     */
    void get_mapper_container_for_shuffler(const matrix_t& hidden_mapper_res);

private:
    // окружение джобы
    JobContext ctx_;

    // содержит в себе результаты работы маппера, шаффлера, редьюсера
    MapReduceContext mr_ctx_;

    // хоолдер для mmap файла
    std::unique_ptr<file_mapping_handler> file_mapping_handler_;

    // FIXME: нам это точно нужно?
    std::mutex reducer_mx;

    friend class JobTester;
};

} // mapreduce

#endif //MAPREDUCE_JOB_H
