#ifndef N_GRAMS_PREDICTION_H
#define N_GRAMS_PREDICTION_H


#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>


std::unordered_map<std::string, double> file_to_probabilities_map(const std::string& filename);

std::unordered_map<std::string, std::vector<std::string>> file_to_next_words_map(const std::string& filename);

std::vector<std::string> predict_next_word(const std::string& phrase, std::unordered_map<std::string, double>& prob_map, std::unordered_map<std::string, std::vector<std::string>>& next_words_map, int words_n);

#endif //N_GRAMS_PREDICTION_H
