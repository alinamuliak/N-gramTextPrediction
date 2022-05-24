// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <unordered_map>
#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <iterator>

#include "../indexing/myqueue.hpp"
#include "../parser/parser.h"
#include "../indexing/processing.h"
#include "../prediction/prediction.h"
#include "main.h"


using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::ref;
using std::unordered_map;
using std::filesystem::path;


void write_to_file(const std::unordered_map<std::string, double> &m, std::ofstream &prob, std::ofstream &next_words, int n) {
    for (auto &p: m) {
        prob << p.first << ":" << p.second << std::endl;

        next_words << get_first_n_words(n - 1, p.first) << ":" << p.first.substr(p.first.find_last_of(' ') + 1) << std::endl;
    }
}


int main(int argc, char *argv[]) {

    boost::locale::localization_backend_manager lbm = boost::locale::localization_backend_manager::global();
    lbm.select("icu");

    boost::locale::generator gen;
    std::locale::global(gen("en_US.UTF-8"));

    enum OUT_CODES {
        ALL_GOOD,
        WRONG_AMOUNT_OF_ARGUMENTS,
        INVALID_ARGUMENTS,
        FAILED_TO_OPEN_CONFIG_FILE,
        ERROR_IN_CONFIG_FILE
    };

//    -------------- ARGUMENT VALIDATION --------------

    if (argc < 1) {
        std::cerr << "ERROR: wrong amount of arguments" << endl;
        return WRONG_AMOUNT_OF_ARGUMENTS;
    }


    string filename;
    if (argc >= 2) {
        filename = argv[1];
    } else if (argc == 1) {
        filename = "./index.cfg";
    } else {
        std::cerr << "ERROR wrong amount of arguments" << endl;
        return WRONG_AMOUNT_OF_ARGUMENTS;
    }

    std::ifstream cf(filename);

    if (!cf.is_open()) {
        std::cerr << "ERROR failed to open config file" << endl;
        return FAILED_TO_OPEN_CONFIG_FILE;
    }
    params parsed_cfg;
    try {
        parsed_cfg = fill_params(cf);
    }
    catch (std::exception &ex) {
        std::cerr << "ERROR " << ex.what() << endl;
        return ERROR_IN_CONFIG_FILE;
    }

    //    ------------------- MAIN PART --------------------

    if (parsed_cfg.option == 0) {
        safe_que<path> files_que(parsed_cfg.files_queue_s);
        safe_que<string> string_que(parsed_cfg.strings_queue_s);
        safe_que<unordered_map<string, int>> merge_q_n(parsed_cfg.merge_queue_s, true);
        safe_que<unordered_map<string, int>> merge_q_n_1(parsed_cfg.merge_queue_s, true);

        // pills
        path empty_p;
        string empty_s;

        auto full_time_start = get_current_time_fenced();

        auto find_time_start = get_current_time_fenced();

        for (const auto &file: std::filesystem::recursive_directory_iterator(parsed_cfg.indir)) {
            files_que.push_end(file.path(), 1, "path");
        }
        files_que.push_end(empty_p, 1, "path");
        auto find_time = get_current_time_fenced() - find_time_start;

        std::vector<std::thread> main_flows(parsed_cfg.index_threads);
        std::vector<std::thread> merge_flows(parsed_cfg.merge_threads);


        for (int i = 0; i < parsed_cfg.index_threads; i++) {
            main_flows.emplace_back(index_string, ref(string_que), ref(merge_q_n), ref(merge_q_n_1),
                                    std::ref(parsed_cfg.extensions), parsed_cfg.ngram_par);
        }
        for (int i = 0; i < parsed_cfg.merge_threads; i++) {
            merge_flows.emplace_back(parallel_merge_maps, ref(merge_q_n));
        }

        for (int i = 0; i < parsed_cfg.merge_threads; i++) {
            merge_flows.emplace_back(parallel_merge_maps, ref(merge_q_n_1));
        }

        auto read_time_start = get_current_time_fenced();

        while (files_que.get_size() > 0) {
            auto file_p = files_que.pop().first;

            if (file_p.empty()) {
                string_que.push_end(empty_s, 1, "poison_pill");
                break;
            }

            // WHY MACOS WHHYYYYYYYY?!?!?!??!?!??!
            if (file_p.extension() == ".DS_Store") {
                continue;
            }

            std::ifstream raw_file(file_p, std::ios::binary);
            std::ostringstream buffer_ss;
            buffer_ss << raw_file.rdbuf();
            std::string buffer{buffer_ss.str()};


            // ingore files > 10MB
            if (buffer.empty() || raw_file.tellg() > 10485760) {
                continue;
            }
            string_que.push_end(buffer, buffer.size(), file_p.extension().string());
        }

        auto read_time = get_current_time_fenced() - read_time_start;

        for (auto &t: main_flows)
            if (t.joinable())
                t.join();

        merge_q_n.push_end(unordered_map<string, int>{}, 1, "poison_pill");
        merge_q_n_1.push_end(unordered_map<string, int>{}, 1, "poison_pill");


        for (auto &t: merge_flows)
            if (t.joinable())
                t.join();

        auto f_map_n = merge_q_n.pop().first;
        auto f_map_n_1 = merge_q_n_1.pop().first;

        auto full_time = get_current_time_fenced() - full_time_start;

        auto write_time_start = get_current_time_fenced();

        unordered_map<string, double> prob_map;
        count_probabilities(prob_map, f_map_n, f_map_n_1, parsed_cfg.ngram_par);
        std::ofstream of_prob(parsed_cfg.out_prob);
        std::ofstream of_next_words(parsed_cfg.out_ngram);
        write_to_file(prob_map, of_prob, of_next_words, parsed_cfg.ngram_par);

        auto write_time = get_current_time_fenced() - write_time_start;

        cout << "Done! <3" << endl;
        cout << "Total time taken: " << to_us(full_time) << endl;
        cout << "\t  Read:   " << to_us(read_time) << endl;
        cout << "\t  Find:   " << to_us(find_time) << endl;
        cout << "\t  Write:  " << to_us(write_time) << endl;

    } else if (parsed_cfg.option == 1) {

        cout << "Hold on... Preparing text..." << endl;
        std::unordered_map<std::string, double> prob_map;
        std::unordered_map<std::string, std::vector<std::string>> next_words_map;

        auto prediction_time_start = get_current_time_fenced();
        if (parsed_cfg.pred_threads == 0) {
            prob_map = file_to_probabilities_map(parsed_cfg.out_prob);
            next_words_map = file_to_next_words_map(parsed_cfg.out_ngram);
        } else {
            // читаємо весь файл і сплітимо по \n
            std::ifstream out_prob(parsed_cfg.out_prob);
            auto probabilities = static_cast<std::ostringstream&>(
                    std::ostringstream{} << out_prob.rdbuf()).str();
            std::ifstream out_ngram(parsed_cfg.out_ngram);
            auto ngram = static_cast<std::ostringstream&>(
                    std::ostringstream{} << out_ngram.rdbuf()).str();

            std::vector<std::string> probabilities_split;
            std::vector<std::string> ngram_split;
            boost::algorithm::split(probabilities_split, probabilities, boost::is_any_of("\n"));
            boost::algorithm::split(ngram_split, ngram, boost::is_any_of("\n"));

            size_t n = ngram_split.size();
            size_t lines_per_thread = std::floor(n / parsed_cfg.pred_threads);


            std::vector<std::thread> processing_flows(parsed_cfg.pred_threads);
            std::vector<std::unordered_map<std::string, std::vector<std::string>>> words_maps;
            std::vector<std::unordered_map<std::string, double>> probability_maps;
            for (int i = 0; i < std::floor(parsed_cfg.pred_threads/2); i++) {
                processing_flows.emplace_back(string_to_next_words_map_parallel, ref(words_maps[i]), probabilities_split, i, lines_per_thread);
            }

            for (int i = 0; i < std::ceil(parsed_cfg.pred_threads/2); ++i) {
                processing_flows.emplace_back(string_to_probabilities_map_parallel, ref(probability_maps[i]), ngram_split, i, lines_per_thread);
            }

            for (int i = 0; i < parsed_cfg.pred_threads; ++i) {
                processing_flows[i].join();
            }

//            merge maps
            prob_map = merge_probability(probability_maps);
            next_words_map = merge_next_words(words_maps);
        }

        auto prediction_time = get_current_time_fenced() - prediction_time_start;
        cout << "Processing time: " << to_us(prediction_time) << endl;

        std::string end_punctuation = ".!?";
        std::string continue_punctuation = ",:;\"'";
        int n = parsed_cfg.ngram_par - 1;
        std::vector<std::string> last_n_inputs;
        while (n--) {
            last_n_inputs.emplace_back("<s>");
        }
        std::string current_input = join(last_n_inputs);

        // для закінчення вводу - ///
        while (current_input != "///") {
            auto predicted_words = predict_next_word(join(last_n_inputs), prob_map, next_words_map, parsed_cfg.word_num);
            for (const auto& el: predicted_words) {
                cout << el << " ";
            }
            cout << endl;
            last_n_inputs.erase(last_n_inputs.begin());
            cout << "->\t";
            cin >> current_input;
            boost::trim(current_input);

            if (end_punctuation.find(current_input) != std::string::npos) {
                last_n_inputs.emplace_back("</s>");
                last_n_inputs.erase(last_n_inputs.begin());
                last_n_inputs.emplace_back("<s>");
                continue;
            } else if (continue_punctuation.find(current_input) != std::string::npos) {
                continue;
            }
            last_n_inputs.emplace_back(current_input);
        }
        cout << "Finish!" << endl;

    } else {
        std::cerr << "Wrong mode: need 0 for 'train' or 1 for 'predict'" << endl;
        return INVALID_ARGUMENTS;
    }

    return ALL_GOOD;
}
