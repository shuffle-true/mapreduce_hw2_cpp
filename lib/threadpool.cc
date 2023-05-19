//
// Created by rozhin on 13.05.23.
//

#include "concurrency/threadpool.h"

namespace mapreduce {

threadpool::threadpool(int32_t num_threads) {
    threads.reserve(num_threads);
    for (int i=0; i<num_threads; i++){
        threads.emplace_back(&threadpool::run, this);
    }
}

threadpool::~threadpool() {
    wait_all();
    quite = true;
    for (uint32_t i = 0; i < threads.size(); ++i) {
        queue_cv.notify_all();
        threads[i].join();
    }
}

bool threadpool::calculated(int32_t task_id) {
        std::lock_guard<std::mutex> lock(completed_task_ids_mtx);
        if (done_ids.find(task_id) != done_ids.end()) {
            return true;
        }
        return false;
}

void threadpool::wait(int task_id) {
    std::unique_lock<std::mutex> lock(completed_task_ids_mtx);

    completed_task_ids_cv.wait(lock, [this, task_id]()->bool {
        return done_ids.find(task_id) != done_ids.end();
    });
}

void threadpool::wait_all() {
    std::unique_lock<std::mutex> lock(queue_mutex);

    // ожидаем вызова notify в функции run (сработает после завершения задачи)
    completed_task_ids_cv.wait(lock, [this]()->bool {
        std::lock_guard<std::mutex> task_lock(completed_task_ids_mtx);
        return tasks_queue.empty() && last_idx == done_ids.size();
    });
}

void threadpool::run() {
    while (!quite){
        std::unique_lock<std::mutex> lock(queue_mutex);
        queue_cv.wait(lock, [this]()->bool{return !tasks_queue.empty() || quite;});

        if (!tasks_queue.empty()){
            auto elem = std::move(tasks_queue.front());
            tasks_queue.pop();
            lock.unlock();

            try {
                elem.first.get();
            }
            catch (std::exception& exc) {
                exc.what();
            }

            std::lock_guard<std::mutex> lock(completed_task_ids_mtx);
            done_ids.insert(elem.second);
            completed_task_ids_cv.notify_all();
        }
    }
}

} // mapreduce