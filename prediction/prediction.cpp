#include "prediction.h"


bool contains(const std::unordered_map<std::string, std::vector<std::string>>& map, const std::string& key) {
    boost::detail::Sleep(100);
    if (map.find(key) == map.end()) {
        return false;
    }
    return true;
}

std::unordered_map<std::string, double> file_to_probabilities_map(const std::string& filename) {
    std::ifstream infile(filename);
    std::unordered_map<std::string, double> probabilities_map;
    std::string line;
    while (std::getline(infile, line)) {
        std::vector<std::string> result;
        boost::algorithm::split(result, line, boost::is_any_of(":"));
        probabilities_map[result[0]] = std::stod(result[1]);
    }
    return probabilities_map;
}


std::unordered_map<std::string, std::vector<std::string>> file_to_next_words_map(const std::string& filename) {
    std::unordered_map<std::string, std::vector<std::string>> words_map;
    std::ifstream infile(filename);
    std::string line;
    while (std::getline(infile, line)) {
        std::vector<std::string> result;
        boost::algorithm::split(result, line, boost::is_any_of(":"));
        words_map[result[0]].emplace_back(result[1]);
    }
    return words_map;
}

std::string predict_next_word(const std::string& phrase, std::unordered_map<std::string, double>& prob_map, std::unordered_map<std::string, std::vector<std::string>>& next_words_map) {
    auto normalized_phrase = boost::locale::fold_case(boost::locale::normalize(phrase));
    if (contains(next_words_map, normalized_phrase)) {
//        тут можна потім додати, щоб пропонувало декілька варіантів продовження, як в телефоні
        double max_probability = 0;
        std::string predicted_word;
        for (const auto& str: next_words_map[normalized_phrase]) {
            std::string current_str = normalized_phrase + " " + str;
//            std::cout << current_str << std::endl;
            if (prob_map[current_str] > max_probability) {
                max_probability = prob_map[current_str];
                predicted_word = str;
            }
        }
        if (predicted_word == "</s>") {
            return "*end of sentence*";     // end of sentence
        }
        return predicted_word;
    }
    std::cout << "Can't predict words for it yet :(" << std::endl;
    return "";
}

