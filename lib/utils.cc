//
// Created by rozhin on 24.05.23.
//

#include "utils/utils.h"

namespace mapreduce {

    // TODO: проверить этот буллщит
size_t get_min_available_threads(size_t first, size_t second) {
    size_t hidden_threads_num = std::min(first, second);
    size_t hardware_threads = std::thread::hardware_concurrency();
    return std::min(hidden_threads_num, hardware_threads);
}

std::vector<std::pair<char, char>> get_reducer_mapping(size_t num_reducer) {
    std::vector<std::pair<char, char>> result_vec;
    constexpr char lang[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr size_t size = sizeof(lang) / sizeof(char);
    for (size_t i = 0; i < size - size / num_reducer; i += size / num_reducer) {
        result_vec.push_back({lang[i], lang[i + size / num_reducer]});
    }
    result_vec.back().second = 'Z';
    return result_vec;
}

}