#ifndef N_GRAMTEXTPREDICTION_PREPROCESS_TEXT_H
#define N_GRAMTEXTPREDICTION_PREPROCESS_TEXT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
//#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include <codecvt>

bool is_newline(char c);
std::string preprocess(const std::string& file_name);

#endif //N_GRAMTEXTPREDICTION_PREPROCESS_TEXT_H
