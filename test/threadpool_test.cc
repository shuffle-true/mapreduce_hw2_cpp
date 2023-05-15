//
// Created by rozhin on 13.05.23.
// Copyright (c) 2021-2023 xsdnn. All rights reserved.
//

#include <gtest/gtest.h>
#include "concurrency/threadpool.h"

TEST(concurrency, thread_pool) {
    tiny::thread_pool pool(8);
    pool.push_task([] {
        std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
    });
}