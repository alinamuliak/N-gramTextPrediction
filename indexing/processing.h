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


std::string get_first_n_words(int n, const std::string &text);

void count_probabilities(std::unordered_map<std::string, double> &prob_map,
                         const std::unordered_map<std::string, int> &phrase_map_n,
                         std::unordered_map<std::string, int> &phrase_map_n_1, int n);

void make_ngrams(std::unordered_map<std::string, int> &ph_map, std::vector<std::string> &w, int n);

void
count_ngrams(std::unordered_map<std::string, int> &phrase_map_n, std::unordered_map<std::string, int> &phrase_map_n_1,
             std::unordered_map<std::string, int> dict_eng, const std::string &line);

void parallel_merge_maps(safe_que<std::unordered_map<std::string, int>> &mer_q);

void index_string(safe_que<std::string> &queue, safe_que<std::unordered_map<std::string, int>> &merge_q_n,
                  safe_que<std::unordered_map<std::string, int>> &merge_q_n_1,
                  const std::unordered_map<std::string, int> &dict_eng, const std::string &ext, int n);

std::unordered_map<std::string, int> file_to_dictionary(const std::string &fileName);

#endif //N_GRAMS_PROCESSING_H
