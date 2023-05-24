//
// Created by rozhin on 15.05.23.
//

#include "utils/job_context.h"

namespace mapreduce {
JobContext::JobContext() = default;
JobContext::JobContext(const std::vector<std::string> &filenames, const size_t num_workers, const size_t num_reducers,
                       const std::string tmp_dir, const std::string out_dir,
                       std::function<void(const char *, size_t, vec_t &)> m_task)
        :
        filenames_(filenames),
        num_workers_(num_workers),
        num_reducers_(num_reducers),
        tmp_dir_(tmp_dir),
        out_dir_(out_dir),
        map_task_(m_task) {}

void JobContext::set_filenames(const std::vector<std::string> &filenames) {
    filenames_ = filenames;
}

void JobContext::set_num_workers(const size_t num_workers) {
    num_workers_ = num_workers;
}

void JobContext::set_num_reducers(const size_t num_reducers) {
    num_reducers_ = num_reducers;
}

void JobContext::set_tmp_dir(const std::string tmp_dir) {
    tmp_dir_ = tmp_dir;
}

void JobContext::set_out_dir(const std::string out_dir) {
    out_dir_ = out_dir;
}

std::vector<std::string> JobContext::get_filenames() const {
    return filenames_;
}

size_t JobContext::get_num_workers() const {
    return num_workers_;
}

size_t JobContext::get_num_reducers() const {
    return num_reducers_;
}

std::string JobContext::get_tmp_dir() const {
    return tmp_dir_;
}

    std::string JobContext::get_out_dir() const {
    return out_dir_;
}


} // mapreduce