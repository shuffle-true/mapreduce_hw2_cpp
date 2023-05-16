//
// Created by rozhin on 15.05.23.
//

#include "job/job.h"

int main() {
    std::vector<std::string> filenames = {
            "1.txt",
            "2.txt",
            "3.txt",
            "4.txt"
    };

    mapreduce::JobContext context(
            filenames,
            12, 12,
            ".", ".", nullptr);

    mapreduce::job job(context);
    job.start();
}