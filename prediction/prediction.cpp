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

std::vector<std::string> predict_next_word(const std::string& phrase, std::unordered_map<std::string, double>& prob_map, std::unordered_map<std::string, std::vector<std::string>>& next_words_map, int words_n) {
    auto normalized_phrase = boost::locale::fold_case(boost::locale::normalize(phrase));
    std::vector<std::string> predicted_words(words_n);
    if (contains(next_words_map, normalized_phrase)) {
        std::vector<double> words_probability(words_n);
        for (const auto& str: next_words_map[normalized_phrase]) {
            std::string current_str = normalized_phrase + " " + str;
            double min_prob = *min_element(words_probability.begin(), words_probability.end());

            if (prob_map[current_str] > min_prob) {
                auto index_of_min = std::find(words_probability.begin(), words_probability.end(), min_prob) - words_probability.begin();
                words_probability[index_of_min] = prob_map[current_str];

                if (str != "</s>") {
                    predicted_words[index_of_min] = str;
                } else {
                    predicted_words[index_of_min] = "*end of sentence*";
                }
            }
        }
    } else {
        std::cout << "Can't predict words for it yet :(" << std::endl;
    }
    return predicted_words;
}

