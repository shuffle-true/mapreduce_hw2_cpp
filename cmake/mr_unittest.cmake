add_subdirectory(${MR_ROOT}/cmake/external/googletest EXCLUDE_FROM_ALL)

enable_testing()

set(MR_TEST_ROOT ${MR_ROOT}/test)

#include_directories(
#        ../inc
#)

macro(AddTest TARGET SOURCES)
    add_executable(${TARGET} ${SOURCES} ${MR_TEST_ROOT}/test_utils.cc)
    target_link_libraries(${TARGET} GTest::gtest_main mr)
    add_test(${TARGET} ${TARGET})
endmacro()

AddTest(
        mr_all_test
        ${MR_TEST_ROOT}/all_test.cc
)