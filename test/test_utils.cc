//
// Created by rozhin on 19.05.23.
//

#include "test_utils.h"

namespace mapreduce {

JobTester::JobTester(mapreduce::JobContext &ctx) : jobber(ctx) {}
JobTester::~JobTester() = default;

void JobTester::test_splits_routine() {
    jobber.split_file_routine();
}

void JobTester::test_map_routine() {
    jobber.split_file_routine();
    jobber.run_map_task();
}

void JobTester::test_shuffler_routine() {
    jobber.split_file_routine();
    jobber.run_map_task();
    jobber.run_shuffler_task();
}

void JobTester::test_reduce_routine() {
    jobber.split_file_routine();
    jobber.run_map_task();
    jobber.run_shuffler_task();
    jobber.run_reducer_task();
}

void JobTester::test_save_routine() {
    jobber.split_file_routine();
    jobber.run_map_task();
    jobber.run_shuffler_task();
    jobber.run_reducer_task();
    jobber.save_to_json();
}

}