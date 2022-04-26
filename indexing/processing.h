#ifndef QLAB_PROCESSING_H
#define QLAB_PROCESSING_H

#include <archive.h>
#include <archive_entry.h>
#include <iostream>
#include <unordered_map>
#include "myqueue.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/locale/generator.hpp>
#include <filesystem>

void parallel_merge_maps(safe_que<std::unordered_map<std::string, int>> &mer_q);

void count_words(std::unordered_map<std::string, int> &local_map, const std::string &line);

void index_string(safe_que<std::string> &queue, safe_que<std::unordered_map<std::string, int>> &merge_q, const std::string &ext);


#endif //QLAB_PROCESSING_H
