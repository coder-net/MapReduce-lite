#ifndef MapReduce_utils_h
#define MapReduce_utils_h

#include "iface.h"

#include <vector>
#include <string>
#include <limits>

class FileReader {
public:
    static std::string Read(const std::string& file_path);
};

class FileWriter {
public:
    static void Write(const std::string& file_path, Yamr& data);
};

class YamrConverter {
public:
    static Yamr ConvertToDummy(const std::string& str);
    static Yamr ConvertToYamr(const std::string& str);
};

class Splitter {
public:
    static std::vector<std::string> Split(char delimiter, const std::string& data, size_t maxsplit = std::numeric_limits<size_t>::max());
};

#endif
