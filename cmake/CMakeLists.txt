cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)
project(mapreduce VERSION 0.0.0 LANGUAGES C CXX)

option(BUILD_TEST ON)

#
#   MR = MapReduce
#
set(MR_ROOT ${PROJECT_SOURCE_DIR}/..)
set(MR_ROOT_SRC ${MR_ROOT}/lib)

include_directories(
        ../inc
)

set(
        mr_concurrency_src
        ${MR_ROOT_SRC}/threadpool.cc
)

add_library(mr
        STATIC
        ${mr_concurrency_src})

if(BUILD_TEST)
    include(mr_unittest.cmake)
endif(BUILD_TEST)
