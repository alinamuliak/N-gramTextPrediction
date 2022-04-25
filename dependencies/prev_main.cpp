#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include "preprocess_text.h"

std::string get_first_n_words(int n, const std::string& text) {
    std::stringstream ss(text);
    std::string result;
    std::string word;
    int i = 0;
    while (i < n && ss >> word) {
        result += word;
        if (i != n - 1) {
            result += " ";
        }
        ++i;
    }
    return result;
}

// todo: тут ймовірно потрібно ще чистити від таких n-gram як (</s>, <s>)
//      + потрібно розібратися, чи треба додавати N разів <s>
int main() {
    int N = 3;
    auto text = preprocess("data/alice.txt");
//    std::string text = "<s> I am singing </s> <s> I am cooking </s> <s> I am cooking </s>";
    std::unordered_map<std::string, double> ngrams_counts;
    std::unordered_map<std::string, double> nminus1_grams_counts;
    std::unordered_map<std::string, std::set<std::string>> after;

    std::vector<std::string> result;
    boost::split(result, text, boost::is_any_of(" "));
    for (int i = N; i <= result.size(); ++i) {
        std::string current_ngram;
        for (int j = i - N ; j < i; ++j) {
            current_ngram += result[j];
            // add to n-1gram_counts
            if (j == i - 2) {
                if (nminus1_grams_counts.find(current_ngram) != nminus1_grams_counts.end()) {
                    nminus1_grams_counts[current_ngram] += 1;
                } else {
                    nminus1_grams_counts[current_ngram] = 1;
                }
                after[current_ngram].insert(result[j + 1]);
//                std::cout << current_ngram << " | " << result[j + 1] << std:: endl;
            }

            if (j < i - 1) {
                current_ngram += " ";
            }
        }
        if (ngrams_counts.find(current_ngram) != ngrams_counts.end()) {
            ngrams_counts[current_ngram] += 1;
        } else {
            ngrams_counts[current_ngram] = 1;
        }
    }


    std::unordered_map<std::string, double> probabilities;
    for (const auto& el: ngrams_counts) {
        probabilities[el.first] = el.second/nminus1_grams_counts[get_first_n_words(N - 1, el.first)];
    }

//    for (const auto& e: probabilities) {
//        std::cout << "P(" << e.first << ") = " << e.second << std::endl;
//    }

//    ----------- e x a m p l e ----------
    std::string past = "Hold your";
    std::cout << "Hold your:" << std::endl;
    std::string next_word;
    double max_prob = 0;
    for (const auto& b: after[past]) {
        if (probabilities[past + " " + b] > max_prob) {
            max_prob = probabilities[past + " " + b];
            next_word = b;
        }
    }
    std::cout << '\t' << next_word << std::endl;
    return 0;
}

// todo: додати опрацювання невідомих слів (ст.40 у підручнику)