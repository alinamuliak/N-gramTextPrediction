#include "processing.h"

using std::string;
using std::unordered_map;
using std::filesystem::path;

void parallel_merge_maps(safe_que<unordered_map<string, int>> &mer_q) {
    for (;;) {
        auto pair1 = mer_q.pop();
        if (pair1.second == "poison_pill") {
            mer_q.push_end(unordered_map<string, int>{}, 1, "poison_pill");
            break;
        }
        auto pair2 = mer_q.pop();
        if (pair2.second == "poison_pill") {
            mer_q.push_start(pair1.first, 1, pair1.second);
            mer_q.push_end(unordered_map<string, int>{}, 1, "poison_pill");
            break;
        }
        auto map1 = pair1.first;
        auto map2 = pair2.first;
        if (map2.size() > map1.size()) {
            std::swap(map2, map1);
        }
        for (const auto &element: map2) {
            map1[element.first] += element.second;
        }
        mer_q.push_start(std::move(map1), 1, "map_m");
    }
}

void make_ngrams(unordered_map<string, int> &ph_map, std::vector<string> &w, int n) {

    for (size_t i = 0; i <= w.size() - n; i++) {
        string phrase;
        for (int j = 0; j < n; j++) {
            if (j == n - 1) {
                phrase += w[i + j];
            } else {
                phrase += w[i + j] + " ";
            }
        }
        ++ph_map[phrase];
    }
}

void count_ngrams(unordered_map<string, int> &phrase_map, const string &line, int num_g) {
    using boost::locale::boundary::ssegment_index;

    ssegment_index splitter_one(boost::locale::boundary::sentence, line.begin(), line.end());
    splitter_one.rule(boost::locale::boundary::sentence_any);
    for (ssegment_index::iterator i = splitter_one.begin(), e = splitter_one.end(); i != e; ++i) {
        string piece = i->str();
        ssegment_index splitter(boost::locale::boundary::word, piece.begin(), piece.end());
        splitter.rule(boost::locale::boundary::word_letters);

        std::vector<string> words;
        for (ssegment_index::iterator j = splitter.begin(), k = splitter.end(); j != k; ++j) {
            string word = boost::locale::fold_case(boost::locale::normalize(j->str()));

            if (j == splitter.begin()) {
                word.insert(0, "<s>");
            }
            words.push_back(word);
        }
        if (!words.empty()) {
            words[words.size() - 1] = words[words.size() - 1] + "</s>";
            make_ngrams(phrase_map, words, num_g);
            make_ngrams(phrase_map, words, num_g-1);
        }
    }

}


void index_string(safe_que<string> &queue, safe_que<unordered_map<string, int>> &merge_q, const string &ext) {
    auto buff = static_cast<char *>(::operator new(11000000));
    for (;;) {
        unordered_map<string, int> local_map{};
        auto element = queue.pop();
        auto line = element.first;
        if (line.empty()) {
            string empty_s;
            queue.push_end(empty_s, 1, "path");
            break;
        }
        if (element.second == ".zip" && ext.find(".zip") != string::npos) {
            struct archive *a;
            struct archive_entry *entry;
            int err_code;

            memset(buff, 0, 11000000);

            a = archive_read_new();
            archive_read_support_filter_all(a);
            archive_read_support_format_zip(a);

            err_code = archive_read_open_memory(a, line.data(), line.size());

            if (err_code != ARCHIVE_OK) {
                continue;
            }
            while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
                if (archive_entry_size(entry) > 0 && archive_entry_size(entry) < 10000000 &&
                    static_cast<path>(archive_entry_pathname(entry)).extension() == ".txt") {
                    auto size = archive_entry_size(entry);
                    archive_read_data(a, buff, size);
                    buff[size] = '\0';
                    count_ngrams(local_map, buff, 1);
                }
            }
            err_code = archive_read_free(a);
            if (err_code != ARCHIVE_OK) {
                continue;
            }
        } else if (element.second == ".txt" && ext.find(".txt") != string::npos) {
            count_ngrams(local_map, line, 1);
        }
        merge_q.push_start(std::move(local_map), 1, "map");
    }
    ::operator delete(buff);
}