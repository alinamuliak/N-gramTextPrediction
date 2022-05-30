#ifndef N_GRAMS_PROCESSING_H
#define N_GRAMS_PROCESSING_H

#include <archive.h>
#include <archive_entry.h>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include "myqueue.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/locale/generator.hpp>
#include <filesystem>

#include "oneapi/tbb/concurrent_hash_map.h"



std::string get_first_n_words(int n, const std::string &text);

void count_probabilities(std::unordered_map<std::string, double> &prob_map,
                         const std::unordered_map<std::string, int> &phrase_map_n,
                         std::unordered_map<std::string, int> &phrase_map_n_1, int n);

void make_ngrams(tbb::concurrent_hash_map<std::string, int> &map, std::vector<std::string> &w, int n);
void make_ngrams(std::unordered_map<std::string, int> &ph_map, std::vector<std::string> &w, int n);

void count_ngrams(tbb::concurrent_hash_map<std::string, int> &g_map_n,
                  tbb::concurrent_hash_map<std::string, int> &g_map_n_1,
                  const std::string &line, std::unordered_map<std::string,
        int> dict_eng, int n);

void index_string(safe_que<std::string> &queue, tbb::concurrent_hash_map<std::string, int> &g_map_n,
                  tbb::concurrent_hash_map<std::string, int> &g_map_n_1,
                  const std::unordered_map<std::string, int> &dict_eng, const std::string &ext, int n);

std::unordered_map<std::string, int> file_to_dictionary(const std::string &fileName);

#endif //N_GRAMS_PROCESSING_H
