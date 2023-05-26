//
// Created by rozhin on 26.05.23.
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

int main() {
    std::vector<std::string> filenames = {
            "./1.txt",
            "./2.txt"
    };

    mapreduce::JobContext context(filenames,
                                  12, 4,
                                  ".", ".",
                                  mapper, reducer);
    mapreduce::job jobber(context);
    jobber.start();
    return 0;
}
