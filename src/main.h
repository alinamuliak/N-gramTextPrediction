#ifndef QLAB_MAIN_H
#define QLAB_MAIN_H

#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <thread>

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D &d) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

#endif //QLAB_MAIN_H
