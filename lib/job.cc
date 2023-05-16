//
// Created by rozhin on 15.05.23.
//

#include "job/job.h"

namespace mapreduce {

job::job(mapreduce::JobContext &context) : ctx(context) {}

void job::start() {
    split_file_routine();
}

void job::split_file_routine() {
    auto& filenames = ctx.filenames_;
    std::string merged_filename = ctx.tmp_dir_ + "/" + "merged.txt";
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


}

} // mapreduce