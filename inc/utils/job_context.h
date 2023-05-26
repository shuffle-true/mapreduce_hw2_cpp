//
// Created by rozhin on 15.05.23.
//

#ifndef MAPREDUCE_JOB_CONTEXT_H
#define MAPREDUCE_JOB_CONTEXT_H

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <mutex>
#include "utils.h"


namespace mapreduce {

class job;

class JobContext {
public:
    JobContext();
    JobContext(const std::vector<std::string>& filenames,
               const size_t num_workers,
               const size_t num_reducers,
               const std::string tmp_dir,
               const std::string out_dir,
               std::function<void(const char*, size_t, vec_t&)> m_task,
               std::function<void(const std::map<std::string_view, std::vector<size_t>>&,
                                    std::map<std::string_view, size_t>&)> r_task);

public:
    void set_filenames(const std::vector<std::string>& filenames);
    void set_num_workers(const size_t num_workers);
    void set_num_reducers(const size_t num_reducers);
    void set_tmp_dir(const std::string tmp_dir);
    void set_out_dir(const std::string out_dir);

    std::vector<std::string> get_filenames() const;
    size_t get_num_workers() const;
    size_t get_num_reducers() const;
    std::string get_tmp_dir() const;
    std::string get_out_dir() const;

private:
    std::vector<std::string> filenames_;
    size_t num_workers_;
    size_t num_reducers_;
    std::string tmp_dir_;
    std::string out_dir_;
    std::function<void(const char*, size_t, vec_t&)> map_task_;
    std::function<void(std::map<std::string_view, std::vector<size_t>>&,
                       std::map<std::string_view, size_t>&)> reduce_task_;

    std::vector<std::pair<const char*, size_t>> in_splits_;

    friend class job;
    friend class JobTester;
};

}

#endif //MAPREDUCE_JOB_CONTEXT_H
