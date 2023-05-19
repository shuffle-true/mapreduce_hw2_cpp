//
// Created by rozhin on 13.05.23.
//

#ifndef MAPREDUCE_HW2_CPP_THREADPOOL_H
#define MAPREDUCE_HW2_CPP_THREADPOOL_H

#include <vector>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_set>
#include <future>


namespace mapreduce {

class threadpool {
public:
    threadpool(int32_t num_threads);
    ~threadpool();

    void run();

    template <typename Func, typename ...Args>
    int add_task(const Func& task_func, Args&&... args){
        int task_idx = last_idx++;

        std::lock_guard<std::mutex> q_lock(queue_mutex);
        tasks_queue.emplace(std::async(std::launch::deferred, task_func, args...), task_idx);

        queue_cv.notify_one();
        return task_idx;
    }

    void wait(int task_id);
    void wait_all();
    bool calculated(int32_t task_id);

private:
    std::vector<std::thread> threads;
    std::atomic<bool> quite{ false };
    std::atomic<int64_t> last_idx = 0;

    std::queue<std::pair<std::future<void>, int>> tasks_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::unordered_set<int> done_ids;

    std::condition_variable completed_task_ids_cv;
    std::mutex completed_task_ids_mtx;

};

} // mapreduce

#endif //MAPREDUCE_HW2_CPP_THREADPOOL_H
