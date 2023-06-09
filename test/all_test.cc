//
// Created by rozhin on 18.05.23.
//

#include "test_utils.h"
#include <gtest/gtest.h>
using namespace mapreduce;

void mapper(const char* key, const size_t value, vec_t& result) {
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

void reducer(const std::map<std::string_view, std::vector<size_t>>& input,
             std::map<std::string_view, size_t>& output) {
    for (const auto& ptr : input) {
        size_t sum = 0;
        for (size_t i = 0; i < ptr.second.size(); ++i) {
            sum += ptr.second[i];
        }
        output.insert({ptr.first, sum});
    }
}

TEST(jobber, check_in_splits) {
    std::vector<std::string> filenames = {
            "../../test/1.txt",
            "../../test/2.txt"
    };

    mapreduce::JobContext context(filenames,
                                  12, 4,
                                  ".", ".",
                                  mapper, reducer);

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

    mapreduce::JobContext context(filenames,
                                  12, 4,
                                  ".", ".",
                                  mapper, reducer);

    mapreduce::JobTester tester(context);
    tester.test_map_routine();
    auto mapper_res = tester.mapper_results();

    ASSERT_EQ(mapper_res.size(), 11);

    ASSERT_EQ(mapper_res[0].size(), 2);
    ASSERT_EQ(mapper_res[0][0].first, "I");
    ASSERT_EQ(mapper_res[0][1].first, "try");

    ASSERT_EQ(mapper_res[1].size(), 2);
    ASSERT_EQ(mapper_res[1][0].first, "To");
    ASSERT_EQ(mapper_res[1][1].first, "understand");

    ASSERT_EQ(mapper_res[2].size(), 3);
    ASSERT_EQ(mapper_res[2][0].first, "What");
    ASSERT_EQ(mapper_res[2][1].first, "the");
    ASSERT_EQ(mapper_res[2][2].first, "difference");

    ASSERT_EQ(mapper_res[3].size(), 1);
    ASSERT_EQ(mapper_res[3][0].first, "Between");

    ASSERT_EQ(mapper_res[4].size(), 1);
    ASSERT_EQ(mapper_res[4][0].first, "Me");

    ASSERT_EQ(mapper_res[5].size(), 1);
    ASSERT_EQ(mapper_res[5][0].first, "And");

    ASSERT_EQ(mapper_res[6].size(), 1);
    ASSERT_EQ(mapper_res[6][0].first, "You");

    ASSERT_EQ(mapper_res[7].size(), 1);
    ASSERT_EQ(mapper_res[7][0].first, "Ok");

    ASSERT_EQ(mapper_res[8].size(), 1);
    ASSERT_EQ(mapper_res[8][0].first, "Answer");

    ASSERT_EQ(mapper_res[9].size(), 3);
    ASSERT_EQ(mapper_res[9][0].first, "Is");
    ASSERT_EQ(mapper_res[9][1].first, "that");
    ASSERT_EQ(mapper_res[9][2].first, "you");

    ASSERT_EQ(mapper_res[10].size(), 3);
    ASSERT_EQ(mapper_res[10][0].first, "A");
    ASSERT_EQ(mapper_res[10][1].first, "bullshit");
    ASSERT_EQ(mapper_res[10][2].first, "boy");
}

TEST(jobber, shuffler) {
    std::vector<std::string> filenames = {
            "../../test/1.txt",
            "../../test/2.txt"
    };

    mapreduce::JobContext context(
            filenames,
            12, 4,
            ".", ".", mapper, reducer);

    mapreduce::JobTester tester(context);
    tester.test_shuffler_routine();
    auto shuffler_res = tester.shuffler_results();

    ASSERT_EQ(shuffler_res.size(), 4);

    ASSERT_EQ(shuffler_res[0].size(), 4);
    ASSERT_EQ(shuffler_res[1].size(), 5);
    ASSERT_EQ(shuffler_res[2].size(), 5);
    ASSERT_EQ(shuffler_res[3].size(), 5);

    ASSERT_EQ(shuffler_res[0]["A"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[0]["And"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[0]["Answer"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[0]["Between"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[1]["I"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[1]["Is"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[1]["Me"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[1]["Ok"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[1]["To"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[2]["What"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[2]["You"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[2]["boy"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[2]["bullshit"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[2]["difference"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[3]["that"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[3]["the"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[3]["try"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[3]["understand"], std::vector<size_t>({1}));
    ASSERT_EQ(shuffler_res[3]["you"], std::vector<size_t>({1}));
}



TEST(jobber, reducer) {
    std::vector<std::string> filenames = {
            "../../test/1.txt",
            "../../test/2.txt"
    };

    mapreduce::JobContext context(
            filenames,
            12, 4,
            ".", ".", mapper, reducer);

    mapreduce::JobTester tester(context);
    tester.test_reduce_routine();
    auto reducer_res = tester.reducer_results();
    ASSERT_EQ(reducer_res.size(), 4);
    ASSERT_EQ(reducer_res[0].size(), 4);
    ASSERT_EQ(reducer_res[1].size(), 5);
    ASSERT_EQ(reducer_res[2].size(), 5);
    ASSERT_EQ(reducer_res[3].size(), 5);
    
    ASSERT_EQ(reducer_res[0]["A"], 1);
    ASSERT_EQ(reducer_res[0]["And"], 1);
    ASSERT_EQ(reducer_res[0]["Answer"], 1);
    ASSERT_EQ(reducer_res[0]["Between"], 1);
    ASSERT_EQ(reducer_res[1]["I"], 1);
    ASSERT_EQ(reducer_res[1]["Is"], 1);
    ASSERT_EQ(reducer_res[1]["Me"], 1);
    ASSERT_EQ(reducer_res[1]["Ok"], 1);
    ASSERT_EQ(reducer_res[1]["To"], 1);
    ASSERT_EQ(reducer_res[2]["What"], 1);
    ASSERT_EQ(reducer_res[2]["You"], 1);
    ASSERT_EQ(reducer_res[2]["boy"], 1);
    ASSERT_EQ(reducer_res[2]["bullshit"], 1);
    ASSERT_EQ(reducer_res[2]["difference"], 1);
    ASSERT_EQ(reducer_res[3]["that"], 1);
    ASSERT_EQ(reducer_res[3]["the"], 1);
    ASSERT_EQ(reducer_res[3]["try"], 1);
    ASSERT_EQ(reducer_res[3]["understand"], 1);
    ASSERT_EQ(reducer_res[3]["you"], 1);
}

TEST(jobber, saver) {
    std::vector<std::string> filenames = {
            "../../test/1.txt",
            "../../test/2.txt"
    };

    mapreduce::JobContext context(
            filenames,
            12, 4,
            ".", ".", mapper, reducer);

    mapreduce::JobTester tester(context);
    tester.test_save_routine();

    std::vector<std::ifstream> in_files;
    in_files.resize(4);
    for (size_t i = 0; i < 4; ++i) {
        std::string filename = "./reducer_" + std::to_string(i);
        in_files[i] = std::ifstream(filename);
    }

    std::vector<json> j_container;
    j_container.resize(4);
    for (size_t i = 0; i < 4; ++i) {
        in_files[i] >> j_container[i];
    }

    ASSERT_EQ(j_container[0]["A"], 1);
    ASSERT_EQ(j_container[0]["And"], 1);
    ASSERT_EQ(j_container[0]["Answer"], 1);
    ASSERT_EQ(j_container[0]["Between"], 1);

    ASSERT_EQ(j_container[1]["I"], 1);
    ASSERT_EQ(j_container[1]["Is"], 1);
    ASSERT_EQ(j_container[1]["Me"], 1);
    ASSERT_EQ(j_container[1]["Ok"], 1);
    ASSERT_EQ(j_container[1]["To"], 1);

    ASSERT_EQ(j_container[2]["What"], 1);
    ASSERT_EQ(j_container[2]["You"], 1);
    ASSERT_EQ(j_container[2]["boy"], 1);
    ASSERT_EQ(j_container[2]["bullshit"], 1);
    ASSERT_EQ(j_container[2]["difference"], 1);

    ASSERT_EQ(j_container[3]["that"], 1);
    ASSERT_EQ(j_container[3]["the"], 1);
    ASSERT_EQ(j_container[3]["try"], 1);
    ASSERT_EQ(j_container[3]["understand"], 1);
    ASSERT_EQ(j_container[3]["you"], 1);
}




