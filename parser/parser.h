#ifndef N_GRAMS_PARSER_H
#define N_GRAMS_PARSER_H

#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <fstream>

using std::string;

struct parsed_line {
    // a pair of 2 values from the TOML string
    string option_name;
    string value;
};

struct params {
    int index_threads{};
    int merge_threads{};
    int pred_threads{};
    string indir;
    string out_prob;
    string out_ngram;
    string extensions;
    size_t files_queue_s;
    size_t strings_queue_s;
    size_t merge_queue_s;
    int ngram_par;
    int option;
    int word_num;
};

parsed_line parse_string(std::string line);

std::unordered_map<std::string, std::string> parse_file(std::ifstream &cf);

params fill_params(std::ifstream &cf);

struct config_error : public std::runtime_error {
    using runtime_error::runtime_error;
};

struct wrong_option_line_error : public config_error {
    using config_error::config_error;
};

struct wrong_option_arg_error : public config_error {
    using config_error::config_error;
};


#endif //N_GRAMS_PARSER_H
