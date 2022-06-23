#pragma once

#include <mutex>
#include <condition_variable>

struct shared_state {

    std::condition_variable renderer_cond_;
    std::mutex renderer_mtx_;

    std::mutex input_mtx_;

};