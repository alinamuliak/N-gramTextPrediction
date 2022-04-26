
#ifndef QLAB_RESULT_FILES_H
#define QLAB_RESULT_FILES_H


#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <algorithm>

static bool key(const std::pair<std::string, int> &a, const std::pair<std::string, int> &b);

void write_to_file_a(const std::unordered_map<std::string, int> &m, std::ofstream &of);

void write_to_file_c(const std::unordered_map<std::string, int> &m, std::ofstream &of);

#endif //QLAB_RESULT_FILES_H
