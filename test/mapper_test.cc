//
// Created by rozhin on 18.05.23.
//

#include "test_utils.h"
#include <gtest/gtest.h>
using namespace mapreduce;

void mapper(const char* key, const size_t value, mapreduce::pair_vec& result) {
    const char* pos = key;
    while(pos - key < value) {
        const char* start_pos = pos;
        while (pos - key < value && *pos != ' ') {
            pos += 1;
        }
        result.push_back(std::string_view(start_pos, pos - start_pos));
        pos += 1;
    }
}

void reducer(mapreduce::map_vec& mapper_output, std::map<std::string_view, int32_t>& result, std::string_view key){
    result.insert({key, 0});
    for(size_t i=0; i<mapper_output.size(); i++){
        for(size_t j=0; j < mapper_output[i].size(); j++){
            if (mapper_output[i][j]==key){
                result[key]++;
            }
        }
    }
}

TEST(jobber, check_in_splits) {
    std::vector<std::string> filenames = {
            "../../test/1.txt",
            "../../test/2.txt"
    };

    mapreduce::JobContext context(
            filenames,
            12, 4,
            ".", ".", mapper, reducer);

    mapreduce::JobTester tester(context);
    tester.test_splits_routine();
    auto in_splits = tester.in_splits();
    ASSERT_EQ(tester.in_splits_size(), 11);

    ASSERT_EQ(std::string_view(in_splits[0].first, in_splits[0].second), "I try");
    ASSERT_EQ(std::string_view(in_splits[1].first, in_splits[1].second), "To understand");
    ASSERT_EQ(std::string_view(in_splits[2].first, in_splits[2].second), "What the difference");
    ASSERT_EQ(std::string_view(in_splits[3].first, in_splits[3].second), "Between");
    ASSERT_EQ(std::string_view(in_splits[4].first, in_splits[4].second), "Me");
    ASSERT_EQ(std::string_view(in_splits[5].first, in_splits[5].second), "And");
    ASSERT_EQ(std::string_view(in_splits[6].first, in_splits[6].second), "You");
    ASSERT_EQ(std::string_view(in_splits[7].first, in_splits[7].second), "Ok");
    ASSERT_EQ(std::string_view(in_splits[8].first, in_splits[8].second), "Answer");
    ASSERT_EQ(std::string_view(in_splits[9].first, in_splits[9].second), "Is that you");
    ASSERT_EQ(std::string_view(in_splits[10].first, in_splits[10].second), "A bullshit boy");
}

TEST(jobber, mapper) {
    std::vector<std::string> filenames = {
            "../../test/1.txt",
            "../../test/2.txt"
    };

    mapreduce::JobContext context(
            filenames,
            12, 4,
            ".", ".", mapper, reducer);

    mapreduce::JobTester tester(context);
    tester.test_map_routine();
    auto mapper_res = tester.mapper_results();

    ASSERT_EQ(mapper_res.size(), 11);

    ASSERT_EQ(mapper_res[0].size(), 2);
    ASSERT_EQ(mapper_res[0][0], "I");
    ASSERT_EQ(mapper_res[0][1], "try");

    ASSERT_EQ(mapper_res[1].size(), 2);
    ASSERT_EQ(mapper_res[1][0], "To");
    ASSERT_EQ(mapper_res[1][1], "understand");

    ASSERT_EQ(mapper_res[2].size(), 3);
    ASSERT_EQ(mapper_res[2][0], "What");
    ASSERT_EQ(mapper_res[2][1], "the");
    ASSERT_EQ(mapper_res[2][2], "difference");

    ASSERT_EQ(mapper_res[3].size(), 1);
    ASSERT_EQ(mapper_res[3][0], "Between");

    ASSERT_EQ(mapper_res[4].size(), 1);
    ASSERT_EQ(mapper_res[4][0], "Me");

    ASSERT_EQ(mapper_res[5].size(), 1);
    ASSERT_EQ(mapper_res[5][0], "And");

    ASSERT_EQ(mapper_res[6].size(), 1);
    ASSERT_EQ(mapper_res[6][0], "You");

    ASSERT_EQ(mapper_res[7].size(), 1);
    ASSERT_EQ(mapper_res[7][0], "Ok");

    ASSERT_EQ(mapper_res[8].size(), 1);
    ASSERT_EQ(mapper_res[8][0], "Answer");

    ASSERT_EQ(mapper_res[9].size(), 3);
    ASSERT_EQ(mapper_res[9][0], "Is");
    ASSERT_EQ(mapper_res[9][1], "that");
    ASSERT_EQ(mapper_res[9][2], "you");

    ASSERT_EQ(mapper_res[10].size(), 3);
    ASSERT_EQ(mapper_res[10][0], "A");
    ASSERT_EQ(mapper_res[10][1], "bullshit");
    ASSERT_EQ(mapper_res[10][2], "boy");
}

// TODO: write test for shuffler && reducer
