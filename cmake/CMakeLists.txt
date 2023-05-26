cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
project(mapreduce VERSION 0.0.0 LANGUAGES C CXX)

option(BUILD_TEST ON)

#
#   MR = MapReduce
#
set(MR_ROOT ${PROJECT_SOURCE_DIR}/..)
set(MR_ROOT_SRC ${MR_ROOT}/lib)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_subdirectory(${MR_ROOT}/cmake/external/json EXCLUDE_FROM_ALL)
include_directories(
        ${MR_ROOT}/cmake/external/json/include
)

# Set flags to build type
set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wall -Wpedantic -Wno-narrowing")
set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE}")
set(EXTRA_C_FLAGS_DEBUG   "${EXTRA_C_FLAGS_DEBUG} -g3 -pg -fsanitize=address -O0")

# Set compiler options
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${EXTRA_C_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${EXTRA_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_C_FLAGS_DEBUG}")

include_directories(
        ../inc
)

set(
        mr_concurrency_src
        ${MR_ROOT_SRC}/threadpool.cc
)

set(
        mr_job_src
        ${MR_ROOT_SRC}/job.cc
        ${MR_ROOT_SRC}/job_context.cc
        ${MR_ROOT_SRC}/mapreduce_context.cc
        ${MR_ROOT_SRC}/file_mapping.cc
)

set(
        mr_utils_src
        ${MR_ROOT_SRC}/utils.cc
)

add_library(mr
        STATIC
        ${mr_concurrency_src}
        ${mr_job_src}
        ${mr_utils_src})

add_executable(mr_main ${MR_ROOT}/test/main.cc)
target_link_libraries(mr_main mr)

if(BUILD_TEST)
    include(mr_unittest.cmake)
endif(BUILD_TEST)
