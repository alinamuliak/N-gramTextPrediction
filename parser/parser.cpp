#include "parser.h"

using std::string;
using std::unordered_map;


parsed_line parse_string(string line) {
    // parse individual line
    // code taken from (inspired by) Oleg Farenyuk's code
    constexpr char separator = '=';
    constexpr char commenter = '#';
    constexpr char inverted_commas = '\"';

    auto comment_pos = line.find(commenter);
    string orig_line = line;
    if (comment_pos != string::npos) {
        line.erase(comment_pos);
    }

    // clear the string
    boost::trim(line);
    line.erase(remove(line.begin(), line.end(), inverted_commas), line.end());

    auto sep_pos = line.find(separator);
    auto option_name = line.substr(0, sep_pos);
    auto value = line.substr(sep_pos + 1, string::npos);
    boost::trim(option_name);
    boost::trim(value);

    if (!line.empty()) {
        if (option_name.empty()) {
            throw wrong_option_line_error("wrong option line : no option name |" + orig_line + "|");
        }
        if (value.empty()) {
            throw wrong_option_arg_error("wrong option value : no option value |" + orig_line + "|");
        }
        if (sep_pos == string::npos) {
            throw wrong_option_line_error("wrong option line : no \"= |" + orig_line + "|");
        }
    }

    return parsed_line{option_name, value};
}

unordered_map<string, string> parse_file(std::ifstream &cf) {
    // parse entire .cfg file
    unordered_map<string, string> results;
    string line;
    while (std::getline(cf, line)) {
        auto option = parse_string(line);

        if (option.option_name == "allowed_ext") {
            results[option.option_name] += " " + option.value;
        } else {
            results[option.option_name] = option.value;
        }
    }
    return results;
}

params fill_params(std::ifstream &cf) {
    unordered_map<string, string> map = parse_file(cf);
    params p{};
    p.index_threads = stoi(map["index_threads"]);
    p.merge_threads = stoi(map["merge_threads"]);
    p.indir = map["indir"];
    p.out_prob = map["out_prob"]; //ofile_a
    p.out_ngram = map["out_ngram"];  //ofile_c
    p.extensions = map["allowed_ext"];
    p.files_queue_s = stol(map["files_queue_s"]);
    p.strings_queue_s = stol(map["strings_queue_s"]);
    p.merge_queue_s = stol(map["merge_queue_s"]);
    p.ngram_par = stoi(map["ngram_par"]);
    p.option = stoi(map["option"]);
    p.word_num = stoi(map["word_num"]);

    return p;
}