add_subdirectory(${MR_ROOT}/cmake/external/googletest EXCLUDE_FROM_ALL)

enable_testing()

set(MR_TEST_ROOT ${MR_ROOT}/test)

macro(AddTest TARGET SOURCES)
    add_executable(${TARGET} ${SOURCES} )
    target_link_libraries(${TARGET} GTest::gtest_main mr)
    add_test(${TARGET} ${TARGET})
endmacro()

AddTest(
        mr_threadpool_test
        ${MR_TEST_ROOT}/threadpool_test.cc
)