
#include "result_files.h"

static bool key(const std::pair<std::string, int> &a, const std::pair<std::string, int> &b) {
    if (a.second != b.second) return a.second > b.second;
    else return a.first < b.first;
}

void write_to_file_a(const std::unordered_map<std::string, int> &m, std::ofstream &of) {
    std::vector<std::pair<std::string, int>> values(m.begin(), m.end());
    std::sort(values.begin(), values.end());
    for (auto &p: m) {
        of << p.first << "  :  " << p.second << std::endl;
    }
}

void write_to_file_c(const std::unordered_map<std::string, int> &m, std::ofstream &of) {
    std::vector<std::pair<std::string, int>> values(m.begin(), m.end());
    std::sort(values.begin(), values.end(), key);

    for (auto &p: values) {
        of << p.first << "  :  " << p.second << std::endl;
    }
}