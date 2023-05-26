//
// Created by rozhin on 19.05.23.
//

#ifndef MAPREDUCE_TEST_UTILS_H
#define MAPREDUCE_TEST_UTILS_H

#include "job/job.h"

namespace mapreduce {

class JobTester {
public:
    explicit JobTester(JobContext& ctx);
    ~JobTester();

    void test_splits_routine();
    void test_map_routine();
    void test_shuffler_routine();
    void test_reduce_routine();

    size_t in_splits_size() const {
        return jobber.ctx_.in_splits_.size();
    }

    std::vector<std::pair<const char *, size_t>> in_splits() const {
        return jobber.ctx_.in_splits_;
    }

    matrix_t mapper_results() const {
        return jobber.mr_ctx_.mapper_results;
    }

    std::vector<std::map<std::string_view, std::vector<size_t>>> shuffler_results() const {
        return jobber.mr_ctx_.shuffler_results;
    }

    std::vector<std::map<std::string_view, size_t>> reducer_results() const {
        return jobber.mr_ctx_.reducer_results;
    }

public:
    job jobber;
};

} // mapreduce

#endif //MAPREDUCE_TEST_UTILS_H
