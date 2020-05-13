#include "word_counter.h"
#include "../src/utils.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_map>


void WordsCounterMapper::MapData(std::vector<std::pair<std::string, std::string>>& mapped_data, const std::string& text) const {
    std::vector<std::string> words = Splitter::Split(' ', text);
    for (auto word = words.begin(); word != words.end(); ++word) {
        mapped_data.emplace_back(std::make_pair(*word, "1"));
    }
}

Yamr WordsCounterMapper::operator () (Yamr& input) {
    std::vector<std::pair<std::string, std::string>> mapped_data;
    for (auto& key_val : input) {
        if (key_val.first == "" && key_val.second == "") {
            throw std::invalid_argument("No text exception\n");
        }
        MapData(mapped_data, key_val.second);
    }
    return mapped_data;
}

Yamr WordsCounterReducer::operator () (Yamr& mapped_data) {
    std::vector<std::pair<std::string, std::string>> answer;
    size_t word_occurences = 0;
    std::string last_key = mapped_data[0].first;
    std::sort(mapped_data.begin(), mapped_data.end());
    for (auto iter = mapped_data.begin(); iter != mapped_data.end(); ++iter) {
        if (iter->first != last_key) {
            answer.push_back(std::make_pair(last_key, std::to_string(word_occurences)));
            last_key = iter->first;
            word_occurences = 1;
        } else {
            ++word_occurences;
        }
        if (iter == mapped_data.end() - 1) {
            answer.push_back(std::make_pair(last_key, std::to_string(word_occurences)));
        }
    }
    
    return answer;
}
