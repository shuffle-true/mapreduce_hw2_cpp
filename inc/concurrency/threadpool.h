//
// Created by rozhin on 13.05.23.
//

#ifndef MAPREDUCE_HW2_CPP_THREADPOOL_H
#define MAPREDUCE_HW2_CPP_THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace tiny {

class thread_pool {
public:
    thread_pool(size_t num_threads);
    ~thread_pool();

public:
    void push_task(const std::function<void()>& task);

private:
    void run();

private:
    std::atomic<bool> terminate {false};
    std::mutex q_mutex;
    std::condition_variable m_condition;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
};

}

#endif //MAPREDUCE_HW2_CPP_THREADPOOL_H
