#ifndef N_GRAMS_PREDICTION_H
#define N_GRAMS_PREDICTION_H


#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "oneapi/tbb/concurrent_hash_map.h"
#include <boost/locale.hpp>


bool contains(const tbb::concurrent_hash_map<std::string, std::vector<std::string>> &map, const std::string &key);

std::string join(const std::vector<std::string> &v);

std::unordered_map<std::string, double> file_to_probabilities_map(const std::string &filename);

void string_to_probabilities_map_parallel(tbb::concurrent_hash_map<std::string, double> &probabilities_map,
                                          std::vector<std::string> &probabilities_split,
                                          size_t thread_num, size_t lines_per_thread);

std::unordered_map<std::string, std::vector<std::string>> file_to_next_words_map(const std::string &filename);

void string_to_next_words_map_parallel(tbb::concurrent_hash_map<std::string, std::vector<std::string>> &words_map,
                                       std::vector<std::string> &words_split,
                                       size_t thread_num, size_t lines_per_thread);

std::vector<std::string> predict_next_word(const std::string &phrase, std::unordered_map<std::string, double> &prob_map,
                                           std::unordered_map<std::string, std::vector<std::string>> &next_words_map,
                                           size_t words_n);

//std::unordered_map<std::string, double> merge_probability(const std::vector<std::unordered_map<std::string, double>>& prob_maps);
//
//std::unordered_map<std::string, std::vector<std::string>> merge_next_words(const std::vector<std::unordered_map<std::string, std::vector<std::string>>>& words_maps);

#endif //N_GRAMS_PREDICTION_H
