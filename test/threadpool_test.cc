//
// Created by rozhin on 13.05.23.
//

#include "concurrency/threadpool.h"
#include "job/job.h"
#include <gtest/gtest.h>
using namespace mapreduce;

void sum(int& ans, std::vector<int>& arr) {
    for (int i = 0; i < arr.size(); ++i) {
        ans += arr[i];
    }
}

class MapTask2 {
public:
    void operator()(const char* key, const size_t value, pair_vec& result) {
        for (size_t i = 0; i < value; ++i) {
            result.push_back(std::string_view (key, value));
        }
    }
};


TEST(threadpool, _01) {
     threadpool tp(3);
     std::vector<int> s1 = { 1, 2, 3 };
     int ans1 = 0;

     std::vector<int> s2 = { 4, 5 };
     int ans2 = 0;

     std::vector<int> s3 = { 8, 9, 10 };
     int ans3 = 0;

     std::vector<int> s4 = { 8, 9, 10, 20 };
     int ans4 = 0;

     MapTask2 mt;
     const char* a;
     size_t b = 1;
     pair_vec res;

//      добавляем в thread_pool выполняться 3 задачи
     auto id1 = tp.add_task(mt, std::ref(a), std::ref(b), std::ref(res));
     auto id2 = tp.add_task(sum, std::ref(ans2), std::ref(s2));
     auto id3 = tp.add_task(sum, std::ref(ans3), std::ref(s3));
     auto id4 = tp.add_task(sum, std::ref(ans4), std::ref(s4));
//
//     tp.wait_all();
//
//
//     std::cout << ans2 << std::endl;
//     std::cout << ans3 << std::endl;
//     std::cout << ans4 << std::endl;
}