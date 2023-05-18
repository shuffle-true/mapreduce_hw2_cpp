//
// Created by rozhin on 15.05.23.
//

#include "job/job.h"

namespace mapreduce {

job::job(mapreduce::JobContext &context) : ctx_(context) {}

void job::start() {
    split_file_routine();
    run_map_task();
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
    auto mapper = ctx_.map_;
    auto& map_result = mr_ctx_.mapper_results;
    map_result.resize(ctx_.in_splits_.size());

    for (size_t i = 0; i < ctx_.in_splits_.size(); ++i) {
        mapper->operator()(ctx_.in_splits_[i].first, ctx_.in_splits_[i].second, map_result[i]);
    }
}

} // mapreduce