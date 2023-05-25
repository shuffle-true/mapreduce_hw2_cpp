//
// Created by rozhin on 24.05.23.
//

#include "test_utils.h"
#include <gtest/gtest.h>
using namespace mapreduce;

void mapper(const char* key, const size_t value, mapreduce::vec_t& result) {
    const char* pos = key;
    while(pos - key < value) {
        const char* start_pos = pos;
        while (pos - key < value && *pos != ' ') {
            pos += 1;
        }
        result.push_back({std::string_view(start_pos, pos - start_pos), 1});
        pos += 1;
    }
}

TEST(jobber, mapper) {
    std::vector<std::string> filenames = {
            "../../test/1.txt",
            "../../test/2.txt"
    };

    mapreduce::JobContext context(filenames,
                                  12, 4,
                                  ".", ".",
                                  mapper);

    mapreduce::JobTester tester(context);
    tester.test_shuffler_routine();
    auto mapper_res = tester.mapper_results();

//    ASSERT_EQ(mapper_res.size(), 11);
//
//    ASSERT_EQ(mapper_res[0].size(), 2);
//    ASSERT_EQ(mapper_res[0][0], "I");
//    ASSERT_EQ(mapper_res[0][1], "try");
//
//    ASSERT_EQ(mapper_res[1].size(), 2);
//    ASSERT_EQ(mapper_res[1][0], "To");
//    ASSERT_EQ(mapper_res[1][1], "understand");
//
//    ASSERT_EQ(mapper_res[2].size(), 3);
//    ASSERT_EQ(mapper_res[2][0], "What");
//    ASSERT_EQ(mapper_res[2][1], "the");
//    ASSERT_EQ(mapper_res[2][2], "difference");
//
//    ASSERT_EQ(mapper_res[3].size(), 1);
//    ASSERT_EQ(mapper_res[3][0], "Between");
//
//    ASSERT_EQ(mapper_res[4].size(), 1);
//    ASSERT_EQ(mapper_res[4][0], "Me");
//
//    ASSERT_EQ(mapper_res[5].size(), 1);
//    ASSERT_EQ(mapper_res[5][0], "And");
//
//    ASSERT_EQ(mapper_res[6].size(), 1);
//    ASSERT_EQ(mapper_res[6][0], "You");
//
//    ASSERT_EQ(mapper_res[7].size(), 1);
//    ASSERT_EQ(mapper_res[7][0], "Ok");
//
//    ASSERT_EQ(mapper_res[8].size(), 1);
//    ASSERT_EQ(mapper_res[8][0], "Answer");
//
//    ASSERT_EQ(mapper_res[9].size(), 3);
//    ASSERT_EQ(mapper_res[9][0], "Is");
//    ASSERT_EQ(mapper_res[9][1], "that");
//    ASSERT_EQ(mapper_res[9][2], "you");
//
//    ASSERT_EQ(mapper_res[10].size(), 3);
//    ASSERT_EQ(mapper_res[10][0], "A");
//    ASSERT_EQ(mapper_res[10][1], "bullshit");
//    ASSERT_EQ(mapper_res[10][2], "boy");
}


