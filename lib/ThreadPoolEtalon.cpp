#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <unordered_set>
#include <future>
#include <map>

class ThreadPool{
    public:

    std::vector<std::thread> threads;
    std::atomic<bool> quite{ false };
    std::atomic<int64_t> last_idx = 0;

    std::queue<std::pair<std::future<void>, int>> tasks_queue; 
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::unordered_set<int> done_ids;

    std::condition_variable completed_task_ids_cv;
    std::mutex completed_task_ids_mtx;

    ThreadPool(int num_threads){
        threads.reserve(num_threads);
        for (int i=0; i<num_threads; i++){
            threads.emplace_back(&ThreadPool::run, this);
        }
    }

    void run(){
        while (!quite){
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [this]()->bool{return !tasks_queue.empty() || quite;});

            if (!tasks_queue.empty()){
                auto elem = std::move(tasks_queue.front());
                tasks_queue.pop();
                lock.unlock();
                elem.first.get();

                std::lock_guard<std::mutex> lock(completed_task_ids_mtx);
                done_ids.insert(elem.second);
                completed_task_ids_cv.notify_all();

            }

        }
    }

    template <typename Func, typename ...Args>
    int add_task(const Func& task_func, Args&&... args){
        int task_idx = last_idx++;

        std::lock_guard<std::mutex> q_lock(queue_mutex);
        tasks_queue.emplace(std::async(std::launch::deferred, task_func, args...), task_idx);
        
        queue_cv.notify_one();
        return task_idx;
    }   

    void wait(int task_id) {
        std::unique_lock<std::mutex> lock(completed_task_ids_mtx);
        
        // ожидаем вызова notify в функции run (сработает после завершения задачи)
        completed_task_ids_cv.wait(lock, [this, task_id]()->bool {
            return done_ids.find(task_id) != done_ids.end(); 
        });
    }   

    void wait_all() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        // ожидаем вызова notify в функции run (сработает после завершения задачи)
        completed_task_ids_cv.wait(lock, [this]()->bool {
            std::lock_guard<std::mutex> task_lock(completed_task_ids_mtx);
            return tasks_queue.empty() && last_idx == done_ids.size();
        });
    }

    bool calculated(int task_id) {
        std::lock_guard<std::mutex> lock(completed_task_ids_mtx);
        if (done_ids.find(task_id) != done_ids.end()) {
            return true;
        }
        return false;
    }

    ~ThreadPool() {
        // можно добавить wait_all() если нужно дождаться всех задачь перед удалением
        wait_all();
        quite = true;
        for (uint32_t i = 0; i < threads.size(); ++i) {
            queue_cv.notify_all();
            threads[i].join();
        }
    }

};


void sum(int& ans, std::vector<int>& arr) {
    for (int i = 0; i < arr.size(); ++i) {
        ans += arr[i];
    }
}

void reducer (std::vector<std::vector<std::string>>& mapper_output, std::map<std::string, int>&result, std::string key){
    result.insert({key, 0});
    for(size_t i=0; i<mapper_output.size(); i++){
        for(size_t j=0; j < mapper_output[i].size(); j++){
            if (mapper_output[i][j]==key){
                result[key]++;
            }
        }
    }
}

std::vector<std::string> shuffler(std::vector<std::vector<std::string>>& mapper_output){
    std::vector<std::string> result;

    for(size_t i=0; i<mapper_output.size(); i++){
        for(size_t j=0; j < mapper_output[i].size(); j++){
            std::string key = mapper_output[i][j];
            if (std::find(result.begin(), result.end(), key) == result.end())
            {
                result.push_back(key);
            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}




int main(){
    std::vector<std::string> mapper1_res = {"hadoop", "check", "hadoop", "hadoop", "check", "hadoop", "hadoop", "check", "hadoop"};
    std::vector<std::string> mapper2_res = {"hadoop", "a", "b", "hadoop", "check", "hadoop", "c", "b", "b"};
    std::vector<std::string> mapper3_res = {"c", "b", "b", "c", "b", "b"};
    std::vector<std::string> mapper4_res = {"hadoop", "check", "hadoop", "c", "b", "b", "hadoop", "check", "hadoop"};
    std::vector<std::vector<std::string>> mapper_out;
    std::map<std::string, int> result;
    
    int num_threads = 5;

    
    mapper_out.push_back(mapper1_res);
    mapper_out.push_back(mapper2_res);
    mapper_out.push_back(mapper3_res);
    mapper_out.push_back(mapper4_res);


    auto tmp = shuffler(mapper_out);

    for (auto& t : tmp)
    std::cout << t << "\n";


    ThreadPool tp(num_threads);

    for(int i=0; i<tmp.size(); ++i){
        auto id = tp.add_task(reducer, std::ref(mapper_out), std::ref(result), tmp[i]);
    }


    for (auto& t : result)
    std::cout << t.first <<"->"<< t.second << "\n";
    


    return 0;
}

// int main() {
//     ThreadPool tp(3);
//     std::vector<int> s1 = { 1, 2, 3 };
//     int ans1 = 0;
    
//     std::vector<int> s2 = { 4, 5 };
//     int ans2 = 0;
    
//     std::vector<int> s3 = { 8, 9, 10 };
//     int ans3 = 0;
		
//     std::vector<int> s4 = { 8, 9, 10, 20 };
//     int ans4 = 0;
		

//     // добавляем в thread_pool выполняться 3 задачи
//     auto id1 = tp.add_task(sum, std::ref(ans1), std::ref(s1));
//     auto id2 = tp.add_task(sum, std::ref(ans2), std::ref(s2));
//     auto id3 = tp.add_task(sum, std::ref(ans3), std::ref(s3));
//     auto id4 = tp.add_task(sum, std::ref(ans4), std::ref(s4));

//     tp.wait_all();

//     std::cout << ans2 << std::endl;
//     std::cout << ans3 << std::endl;
//     std::cout << ans4 << std::endl;
//     return 0;
// }

