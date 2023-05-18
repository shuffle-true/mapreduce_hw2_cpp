//
// Created by rozhin on 18.05.23.
//

#include "job/job.h"
#include <gtest/gtest.h>
using namespace mapreduce;

class MapTask : public map_task_base {
public:
    typedef const char* key_type;
    typedef size_t      value_type;

public:
    void operator()(const char* key, const size_t value, pair_vec& result) {
        for (size_t i = 0; i < value; ++i) {
            result.push_back(std::string_view (key, value));
        }
    }

};

TEST(mapper, vanilla_1) {
    std::vector<std::string> filenames = {
            "1.txt",
            "2.txt",
            "3.txt",
            "4.txt"
    };

    MapTask mapper;

    mapreduce::JobContext context(
            filenames,
            12, 12,
            ".", ".", &mapper);

    mapreduce::job job(context);
    job.start();
}
