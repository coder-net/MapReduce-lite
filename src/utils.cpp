#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>


std::string FileReader::Read(const std::string& file_path) {
    std::ifstream stream(file_path, std::ifstream::in);
    if (!stream.is_open()) {
        throw std::runtime_error("No such file: " + file_path);
    }
    std::string input((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    return input;
}

void FileWriter::Write(const std::string& file_path, Yamr& data) {
    std::ofstream stream(file_path, std::ofstream::out);
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + file_path);
    }
    for (const auto& p : data) {
        std::stringstream res;
        res << p.first << '\t' << p.second << '\n';
        stream << res.str();
    }
}

Yamr YamrConverter::ConvertToDummy(const std::string& str) {
    return {std::make_pair("", str)};
}

Yamr YamrConverter::ConvertToYamr(const std::string& str) {
    auto lines = Splitter::Split('\n', str);
    Yamr res;
    for (auto& line : lines) {
        if (line.empty()) {
            continue;
        }
        auto v = Splitter::Split('\t', line);
        res.push_back(std::make_pair(v[0], v[1]));
    }
    return res;
}

std::vector<std::string> Splitter::Split(char delimiter, const std::string& data, size_t maxsplit) {
    std::vector<std::string> res;
    size_t start = 0;
    size_t end = 0;
    while (res.size() < maxsplit && (end = data.find(delimiter, start)) != std::string::npos) {
        auto token = data.substr(start, end - start);
        if (!token.empty()) {
            res.push_back(token);
        }
        start = end + 1;
    }
    if (start != std::string::npos) {
        res.push_back(data.substr(start, data.size() - start));
    }
    return res;
}
