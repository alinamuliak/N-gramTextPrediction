#ifndef QUEUE_FILES_H
#define QUEUE_FILES_H

#include <deque>
#include <map>
#include <filesystem>
#include <chrono>
#include <atomic>


#include "../includes/parser.h"
#include <boost/program_options.hpp>
#include <boost/locale.hpp>
#include <archive.h>
#include <archive_entry.h>

namespace sys = std::filesystem;

std::string read_binary_file(const sys::path& path);
std::string extract_archive_files(const std::string& file);
void extract_files(const sys::path& path, std::deque<sys::path>* deque, size_t max_file_size);
void read_files(std::deque<sys::path>* deque, std::deque<std::pair<sys::path, std::string>>* text_deque);
std::map<std::string, int> split(const std::string* str, const std::locale& loc);
void merge(const std::map<std::string, int>& local, std::map<std::string, int>* global);
bool compare(const std::pair<std::string, int>& first, const std::pair<std::string, int>& second, int param);
std::vector<std::pair<std::string, int>> sort_by_func(const std::map<std::string, int>& words, int method);
void write(const sys::path& name, const std::vector<std::pair<std::string, int>>& words);

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template <class D>
long long to_us(const D &d)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

#endif //QUEUE_FILES_H
