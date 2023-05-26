//
// Created by rozhin on 15.05.23.
//

#include "job/job.h"

namespace mapreduce {

job::job(mapreduce::JobContext &context) : ctx_(context) {}

void job::start() {
    split_file_routine();
    run_map_task();
    run_shuffler_task();
    run_reducer_task();
    save_to_json();
}

void job::split_file_routine() {
    auto& filenames = ctx_.filenames_;
    std::string merged_filename = ctx_.tmp_dir_ + "/" + "merged.txt";
    std::ofstream merged(merged_filename);
    std::ostreambuf_iterator<char> it(merged);

    for (size_t i = 0; i < filenames.size(); ++i) {
        std::ifstream in(filenames[i]);
        std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), it);
    }
    merged.close();

    int32_t fd = open(merged_filename.data(), O_RDONLY, 0);

    if (fd < 0) {
        std::cerr << "Open failed, filename: "
                  << merged_filename << ". " << strerror(errno) << std::endl;
        throw std::runtime_error("");
    }

    struct stat st;
    if(fstat(fd, &st) < 0) {
        std::cerr << "fstat failed, filename: "
                  << merged_filename << ". " << strerror(errno) << std::endl;
        close(fd);
        throw std::runtime_error("");
    }

    size_t fsize = (size_t)st.st_size;

    char* dataPtr = (char*)mmap(nullptr, fsize,
                                                  PROT_READ,
                                                  MAP_PRIVATE,
                                                  fd, 0);
    if(dataPtr == MAP_FAILED) {
        std::cerr << "mmap failed, filename: "
                  << merged_filename << ". " << strerror(errno) << std::endl;
        close(fd);
        throw std::runtime_error("");
    }

    file_mapping_handler_ = std::make_unique<file_mapping_handler>(fd, fsize, dataPtr);

    auto& in_splits = ctx_.in_splits_;
    if (fsize < N_BYTES_SPLIT_SIZE) {
        in_splits.resize(1);
        in_splits.at(0) = {file_mapping_handler_->data_, file_mapping_handler_->f_size_};
    } else {
        const char* ptr = file_mapping_handler_->data_;
        while (ptr - file_mapping_handler_->data_ < file_mapping_handler_->f_size_) {
            const char* start_pos = ptr;
            ptr += N_BYTES_SPLIT_SIZE;
            while (*ptr != '\n') {
                ptr++;
            }
            in_splits.push_back({start_pos, ptr - start_pos});
            ptr++;
        }
    }
}

void job::run_map_task() {
    auto& mapper = ctx_.map_task_;
    auto& map_res = mr_ctx_.mapper_results;
    map_res.resize(ctx_.in_splits_.size());
    threadpool tp(get_min_available_threads(ctx_.num_workers_, ctx_.in_splits_.size()));

    // кладем все в тредпул и ждем завершения всех задач
    for (size_t i = 0; i < ctx_.in_splits_.size(); ++i) {
        tp.add_task(
                mapper,
                std::ref(ctx_.in_splits_[i].first),
                std::ref(ctx_.in_splits_[i].second),
                std::ref(map_res[i])
        );
    }
    tp.wait_all();
}

void job::get_mapper_container_for_shuffler(const matrix_t& hidden_mapper_res) {
//    auto& map_res = mr_ctx_.mapper_results;
//    // размер матчится по мапперам
//    map_res.resize(ctx_.in_splits_.size());
//
//    // это измерение всегда матчится по кол-ву редьюсеров
//    for (auto& mat : map_res) {
//        mat.resize(ctx_.num_reducers_);
//    }
//    auto alphabet = get_reducer_mapping(ctx_.num_reducers_);
//    assert(alphabet.size() == ctx_.num_reducers_);
//
//    for (size_t i = 0; i < hidden_mapper_res.size(); ++i) {
//        for (size_t j = 0; j < hidden_mapper_res[i].size(); ++j) {
//            auto first_sym = toupper(hidden_mapper_res[i][j].first[0]);
//            for (size_t r = 0; r < ctx_.num_reducers_; ++r) {
//                if (char(first_sym) >= alphabet[r].first
//                        && char(first_sym) < alphabet[r].second) {
//                    map_res[i][r].push_back(hidden_mapper_res[i][j]);
//                }
//            }
//        }
//    }
}

void job::run_shuffler_task() {
    // Создание словаря уникальных первых символов

    std::list<std::string_view> vocab;
    std::list<std::string_view> ::iterator itr = vocab.begin();
    auto& mapper_output = mr_ctx_.mapper_results;

    itr++;
    vocab.insert(itr, mapper_output[0][0].first);

    for(int i=0; i<mapper_output.size(); i++){
        for(int j=0; j<mapper_output[i].size(); j++){
            std::string_view word = mapper_output[i][j].first;
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
    int batch_size = vocab.size() / ctx_.num_reducers_;
    while (((vocab.size()-batch_size)%(ctx_.num_reducers_-1)!=0)) batch_size+=1;


    itr = vocab.begin();
    std::map<std::string_view, std::vector<size_t>> reducer_temp;
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

    mr_ctx_.shuffler_results.push_back(reducer_temp);

    batch_size = (vocab.size()-batch_size)/(ctx_.num_reducers_-1);

    for(int k=0; k<batch_size*(ctx_.num_reducers_-1); k+=batch_size){
        std::map<std::string_view, std::vector<size_t>> reducer_temp_l;
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
        mr_ctx_.shuffler_results.push_back(reducer_temp_l);
    }
}

void job::run_reducer_task() {
    auto reducer = ctx_.reduce_task_;
    auto& reducer_res = mr_ctx_.reducer_results;
    reducer_res.resize(ctx_.num_reducers_);
    threadpool tp(get_min_available_threads(ctx_.num_reducers_, ctx_.num_workers_));

    // кладем все в тредпул и ждем завершения всех задач
    for (size_t i = 0; i < mr_ctx_.shuffler_results.size(); ++i) {
        tp.add_task(
                reducer,
                std::ref(mr_ctx_.shuffler_results[i]),
                std::ref(reducer_res[i])
        );
    }
    tp.wait_all();
}

void job::save_to_json() {
    std::vector<json> j_container;
    j_container.resize(ctx_.num_reducers_);

    for (size_t i = 0; i < j_container.size(); ++i) {
        for (const auto& ptr : mr_ctx_.reducer_results[i]) {
            j_container[i][ptr.first] = ptr.second;
        }
    }

    std::vector<std::ofstream> out_vec;
    out_vec.resize(ctx_.num_reducers_);

    for (size_t i = 0; i < out_vec.size(); i++) {
        std::string filename = ctx_.out_dir_ + "/reducer_" + std::to_string(i);
        out_vec[i] = std::ofstream(filename);
    }

    for (size_t i = 0; i < out_vec.size(); ++i) {
        out_vec[i] << std::setw(4) << j_container[i] << std::endl;
    }
}

} // mapreduce