//
// Created by rozhin on 15.05.23.
//

#ifndef MAPREDUCE_UTILS_H
#define MAPREDUCE_UTILS_H

#include <vector>
#include <string_view>
#include <thread>

namespace mapreduce {

typedef std::vector<std::pair<std::string_view, size_t>> vec_t;
typedef std::vector<vec_t> matrix_t;
typedef std::vector<matrix_t> tensor_t;

size_t get_min_available_threads(size_t first, size_t second);

// получить границы символов по алфавиту для распределения по редьюсерам
std::vector<std::pair<char, char>> get_reducer_mapping(size_t num_reducer);
} // mapreduce

#endif //MAPREDUCE_UTILS_H
