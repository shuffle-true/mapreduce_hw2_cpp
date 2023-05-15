//
// Created by rozhin on 13.05.23.
//

#include "concurrency/threadpool.h"

namespace tiny {

thread_pool::thread_pool(size_t num_threads) {
    size_t hardware_threads = std::thread::hardware_concurrency();
    if (num_threads < hardware_threads) {
        hardware_threads = num_threads;
    }

    threads.reserve(hardware_threads);
    for (size_t i = 0; i < hardware_threads; ++i) {
        threads.emplace_back(&thread_pool::run, this);
    }
}

void thread_pool::push_task(const std::function<void()> &task) {
    {
        std::unique_lock<std::mutex> lock(q_mutex);
        tasks.push(task);
    }
    m_condition.notify_one(); // проснется один поток для выполнения таски
}

void thread_pool::run() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(q_mutex);
            m_condition.wait(lock, [this] {
               return !tasks.empty() || terminate;
            });

            if (terminate) {
                return;
            }

            job = tasks.front();
            tasks.pop();
        }
        job();
    }
}

thread_pool::~thread_pool() {
    terminate = true;
    for (size_t i = 0; i < threads.size(); ++i) {
        m_condition.notify_all();
        threads[i].join();
    }
}

}