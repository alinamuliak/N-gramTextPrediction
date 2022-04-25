#include "preprocess_text.h"

bool is_newline(char c) {
    return c == '\n';
}

std::string preprocess(const std::string& file_name) {
    std::ifstream file(file_name);

    if (!file) {
        std::cerr << "Error opening file" << std::endl;
        // todo: error here.
    }

    auto text = dynamic_cast<std::ostringstream&>(
            std::ostringstream{} << file.rdbuf()).str();
    // todo: read as utf-8 without creepy symbols
    // also maybe there is more efficient way to remove punctuation
    text.erase(std::remove_if(text.begin(), text.end(), ispunct), text.end());
    // todo: add cleaning from redundant whitespaces
    boost::replace_all(text, "\n", " </s> <s> ");
    text = "<s> " + text + " </s>";
    return text;
}

