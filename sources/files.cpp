#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <deque>
#include <map>
#include <algorithm>
#include <boost/locale.hpp>
#include "../includes/errors.h"
#include "../includes/files.h"

namespace sys = std::filesystem;

std::string read_binary_file(const sys::path& path) {
    std::ifstream raw_file(path, std::ios::binary);
    auto buffer = [&raw_file]{
        std::ostringstream ss{};
        ss << raw_file.rdbuf();
        return ss.str();
    }();
    return buffer;
}



std::string extract_archive_files(const std::string& file) {
    std::vector<std::string> archive_filenames;
    struct archive *archive = archive_read_new();
    struct archive_entry *entry;
    archive_read_support_format_all(archive);

    int result = archive_read_open_memory(archive, file.data(), file.size());

    if (result != ARCHIVE_OK) {
        std::cerr << "Cannot read archive" << std::endl;
        exit(1);
    }
    std::string all_files_data;
    while(archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
        if (static_cast<sys::path>(archive_entry_pathname(entry)).extension() == ".txt" &&
            archive_entry_size(entry) > 0) {

            char* data = new char[10000000];
            archive_read_data(archive, data, archive_entry_size(entry));
            all_files_data += data;
            delete[](data);
        }
    }
    result = archive_read_free(archive);
    if (result != ARCHIVE_OK) {
        std::cerr << "Cannot free archive" <<std::endl;
        exit(1);
    }
    return all_files_data;
}


void extract_files(const sys::path& path, std::deque<sys::path>* deque, size_t max_file_size){
    for (const auto & file : sys::recursive_directory_iterator(path)) {
        if ((file.path().extension().string() == ".txt" || file.path().extension().string() == ".zip")
                && (sys::file_size(file.path()) > 0) && (sys::file_size(file.path()) < max_file_size)) {
            deque->push_back(file.path().string());
        }
    }
}

void read_files(std::deque<sys::path>* deque, std::deque<std::pair<sys::path, std::string>>* text_deque){
    for (auto& elem : *deque){
        try{
            auto str = read_binary_file(elem);
            text_deque->push_back(std::pair(elem, str));
        }
        catch (...) {
            std::cerr << "Cannot read file " << elem << std::endl;
        }
    }
}



std::map<std::string, int> split(const std::string* str, const std::locale& loc){
    std::map<std::string, int> words;

    try {
        auto normalized_fold_case_str = boost::locale::fold_case(boost::locale::normalize(*str, boost::locale::norm_default, loc), loc);
        boost::locale::boundary::ssegment_index map(boost::locale::boundary::word, normalized_fold_case_str.begin(), normalized_fold_case_str.end(), boost::locale::boundary::word_letters, loc);
        for (auto word : map){
            ++words[word];
        }
    } catch (...) {
        std::cout << "Unicode crached file" << std::endl;
    }
    return words;
}

void merge(const std::map<std::string, int>& local, std::map<std::string, int>* global){
    std::map<std::string, int>& global_ref = *global;
    for (auto const& elem: local){
        global_ref[elem.first] += elem.second;
    }
}

bool compare(const std::pair<std::string, int>& first, const std::pair<std::string, int>& second, int param){
    if (param){
        return (first.first < second.first);
    }
    else{
        if (first.second != second.second){
            return (first.second > second.second);
        }
        return (first.first < second.first);
    }
}


std::vector<std::pair<std::string, int>> sort_by_func(const std::map<std::string, int>& words, int method){
    std::vector<std::pair<std::string, int>> sorted;
    for (auto &pair : words){
        sorted.emplace_back(pair);
    }
    std::sort(sorted.begin(), sorted.end(), [method](auto i, auto j){return compare(i, j, method);} );
    return sorted;
}

void write(const sys::path& name, const std::vector<std::pair<std::string, int>>& words){
    std::ofstream file_1(name);
    if (!file_1) {
        std::cerr << "Не вдалося відкрити файл для запису результату!" << std::endl;
        exit(RESULT_FILE_OPEN_ERROR);
    }
    for (auto &pair: words){
        file_1 << pair.first << " " << pair.second << std::endl;
    }

    if (file_1.fail()) {
        std::cerr << "Помилка запису у вихідний файл!" << std::endl;
        exit(RESULT_WRITING_ERROR);
    }
    file_1.close();
}
