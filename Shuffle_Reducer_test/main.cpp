#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <unordered_set>
#include <future>
#include <map>
#include <list>


#define mapper_output_type std::vector<std::vector<std::pair<std::string, int>>>
#define shuffler_output_type std::vector<std::map<std::string, std::vector<int>>>

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


void shuffler(mapper_output_type & mapper_output, shuffler_output_type & result, int &num_reducers){

    // Создание словаря уникальных первых символов

    std::list<std::string> vocab;
    std::list<std::string> ::iterator itr = vocab.begin();

    itr++;
    vocab.insert(itr, mapper_output[0][0].first);

    for(int i=0; i<mapper_output.size(); i++){
        for(int j=0; j<mapper_output[i].size(); j++){
            std::string word = mapper_output[i][j].first;
            bool added = false;

            for(itr = vocab.begin(); itr != vocab.end(); ++itr){
                if (word.compare(*itr)==0){
                    added = true;
                    break;
                }
                else if (word.compare(*itr)<0){
                    vocab.insert(itr, word);
                    added = true;
                    break;
                }
            }
            if(!added){
                vocab.insert(itr, word);
            } 
        }
    }

    // Распаковка по редьюсерам
    
    int element_num = 0;
    int batch_size = vocab.size()/num_reducers;
    while (((vocab.size()-batch_size)%(num_reducers-1)!=0)) batch_size+=1;


    itr = vocab.begin();
    std::map<std::string, std::vector<int>> reducer_temp;
    while(element_num<batch_size){
        reducer_temp[*itr] = {};
        for(int i=0; i<mapper_output.size(); i++){
            for(int j=0; j<mapper_output[i].size(); j++){
                if(mapper_output[i][j].first == *itr){
                    reducer_temp[*itr].push_back(1);
                }
            }
        }
        element_num += 1;
        itr++;
    }

    result.push_back(reducer_temp);

    batch_size = (vocab.size()-batch_size)/(num_reducers-1);

    for(int k=0; k<batch_size*(num_reducers-1); k+=batch_size){
        std::map<std::string, std::vector<int>> reducer_temp_l;
        for( int b=0; b<batch_size; b++){
            reducer_temp_l[*itr] = {};
            for(int i=0; i<mapper_output.size(); i++){
                for(int j=0; j<mapper_output[i].size(); j++){
                    if(mapper_output[i][j].first == *itr){
                        reducer_temp_l[*itr].push_back(1);
                    }
                }
            }
            itr++;
        }
        result.push_back(reducer_temp_l);

    }
}


int main(){

    int num_reducers = 4;

    std::vector<std::pair<std::string, int>> mapper_output1 = {
        {"Anna", 1}, {"Anna", 1}, {"Hello", 1}, {"Check", 1},  {"Anna", 1},{"Abra", 1}, {"Test", 1},  {"Monika", 1}
    };
    std::vector<std::pair<std::string, int>> mapper_output2 = {
        {"Check", 1}, {"Hello", 1}, {"Hello", 1}, {"Test", 1},  {"Check", 1}, {"Test", 1}, {"Joe", 1}, {"Ross", 1}, {"Rachel", 1}
    };
    std::vector<std::pair<std::string, int>> mapper_output3 = {
         {"Hello", 1}, {"Check", 1}, {"Anna", 1}, {"Anna", 1}, {"Hello", 1}, {"Check", 1},  {"Anna", 1}, {"Zorro", 1}, {"MammaMia", 1}
    };

    mapper_output_type complete_output;
    complete_output.push_back(mapper_output1);
    complete_output.push_back(mapper_output2);
    complete_output.push_back(mapper_output3);


    shuffler_output_type shuffler_result;
    shuffler(complete_output, shuffler_result, num_reducers);

    for(int i=0; i<shuffler_result.size(); i++){
        for(const auto& elem : shuffler_result[i])
        {
            std::cout << elem.first << "\n";
        }
        std::cout << "=====" << "\n";
    }

    return 0;
    
}

