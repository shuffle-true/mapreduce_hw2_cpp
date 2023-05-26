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

void reducer(map_vec& mapper_output, std::map<std::string_view, size_t>& result, std::string_view key, std::mutex& mx){
    int temp = 0;
    for(size_t i=0; i<mapper_output.size(); i++){
        for(size_t j=0; j < mapper_output[i].size(); j++){
            if (mapper_output[i][j]==key){
                temp+=1;
            }
        }
    }
    mx.lock();
    result.insert({key, 0});
    result[key] = temp;
    mx.unlock();
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
    std::vector<std::string> shuffler_result;

    for(int i=0; i<shuffler_res.size(); i++){
        for(const auto& elem : shuffler_res[i])
        {

            shuffler_result.push_back(elem.first());
        }
        std::cout << "=====" << "\n";
    }


    ASSERT_EQ(shuffler_res.size(), 4);
    ASSERT_EQ(shuffler_res[0].size(), 4);
    ASSERT_EQ(shuffler_res[1].size(), 5);
    ASSERT_EQ(shuffler_res[2].size(), 5);
    ASSERT_EQ(shuffler_res[3].size(), 5);



    ASSERT_EQ(shuffler_result[0], "A");
    ASSERT_EQ(shuffler_result[1], "And");
    ASSERT_EQ(shuffler_result[2], "Answer");
    ASSERT_EQ(shuffler_result[3], "Between");
    ASSERT_EQ(shuffler_result[4], "I");
    ASSERT_EQ(shuffler_result[5], "Is");
    ASSERT_EQ(shuffler_result[6], "Me");
    ASSERT_EQ(shuffler_result[7], "Ok");
    ASSERT_EQ(shuffler_result[8], "To");
    ASSERT_EQ(shuffler_result[9], "What");
    ASSERT_EQ(shuffler_result[10], "You");
    ASSERT_EQ(shuffler_result[11], "boy");
    ASSERT_EQ(shuffler_result[12], "bullshit");
    ASSERT_EQ(shuffler_result[13], "difference");
    ASSERT_EQ(shuffler_result[14], "that");
    ASSERT_EQ(shuffler_result[15], "the");
    ASSERT_EQ(shuffler_result[16], "try");
    ASSERT_EQ(shuffler_result[17], "understand");
    ASSERT_EQ(shuffler_result[18], "you");
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

    std::pair<std::string, std::int> reducer_final;

    for(int i =0; i<reducer_results.size(); i++){
        for (const auto &ptr : reducer_output[i]){
            reducer_final[ptr.first] = ptr.second;
        }
    }

    ASSERT_EQ(reducer_res.size(), 19);
    ASSERT_EQ(reducer_res["A"], 1);
    ASSERT_EQ(reducer_res["And"], 1);
    ASSERT_EQ(reducer_res["Answer"], 1);
    ASSERT_EQ(reducer_res["Between"], 1);
    ASSERT_EQ(reducer_res["I"], 1);
    ASSERT_EQ(reducer_res["Is"], 1);
    ASSERT_EQ(reducer_res["Me"], 1);
    ASSERT_EQ(reducer_res["Ok"], 1);
    ASSERT_EQ(reducer_res["To"], 1);
    ASSERT_EQ(reducer_res["What"], 1);
    ASSERT_EQ(reducer_res["You"], 1);
    ASSERT_EQ(reducer_res["boy"], 1);
    ASSERT_EQ(reducer_res["bullshit"], 1);
    ASSERT_EQ(reducer_res["difference"], 1);
    ASSERT_EQ(reducer_res["that"], 1);
    ASSERT_EQ(reducer_res["the"], 1);
    ASSERT_EQ(reducer_res["try"], 1);
    ASSERT_EQ(reducer_res["understand"], 1);
    ASSERT_EQ(reducer_res["you"], 1);
}




