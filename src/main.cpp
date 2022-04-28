// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <unordered_map>
#include <boost/locale.hpp>
//#include <boost/locale/generator.hpp>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <iterator>

#include "../indexing/myqueue.hpp"
#include "main.h"
#include "../parser/parser.h"
#include "../indexing/processing.h"
#include "../prediction/prediction.h"


using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::ref;
using std::unordered_map;
using std::filesystem::path;


void
write_to_file(const std::unordered_map<std::string, double> &m, std::ofstream &prob, std::ofstream &next_words, int n) {
    for (auto &p: m) {
        prob << p.first << ":" << p.second << std::endl;

        next_words << get_first_n_words(n - 1, p.first) << ":" << p.first.substr(p.first.find_last_of(' ') + 1)
                   << std::endl;
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

        vector<std::thread> main_flows(parsed_cfg.index_threads);
        vector<std::thread> merge_flows(parsed_cfg.merge_threads);


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

        auto prob_map = file_to_probabilities_map(parsed_cfg.out_prob);
        auto next_words_map = file_to_next_words_map(parsed_cfg.out_ngram);


        // todo: тут поки що тільки для 2-грам зроблено, загальнішу логіку треба доробляти
        std::string input = "<s>";
        std::string prev_input;
        cout << "Start typing: " << endl;
        // для закінчення вводу - ///
        while (input != "///") {
            // todo:: додати гарну обробку всіх розділових знаків
            if (input == "." || input == "!" || input == "?") {
                input = "<s>";
            } else if (input == "," || input == ";" || input == ":" || input == "\"") {
                input = prev_input;
                continue;
            }
            auto predicted_words = predict_next_word(input, prob_map, next_words_map, parsed_cfg.word_num);
            cout << "-> ";
            for (const auto &el: predicted_words) {
                cout << el << " ";
            }
            cout << endl;
            prev_input = input;
            cin >> input;
        }
        cout << "Finish" << endl;

    } else {
        std::cerr << "Wrong mode: need 0 for 'train' or 1 for 'predict'" << endl;
        return INVALID_ARGUMENTS;
    }

    return ALL_GOOD;
}
