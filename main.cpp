#include "examples/word_counter.h"
#include "src/map_reduce.h"
#include "src/utils.h"

#include <iostream>
#include <string>
#include <chrono>

using namespace std;

std::ostream& operator<<(std::ostream& out, const Yamr& yamr) {
    for (const auto& p : yamr) {
        out << p.first << " " << p.second << std::endl;
    }
    return out;
}

chrono::high_resolution_clock::time_point StartTimer(const std::string& msg) {
    cout << msg << endl;
    return chrono::high_resolution_clock::now();
}

void EndTimer(const std::string& msg, chrono::high_resolution_clock::time_point start) {
    const auto end = chrono::high_resolution_clock::now();
    double dur = chrono::duration_cast<chrono::seconds>(end - start).count();
    std::cout << msg << endl << "Duration: " << dur << "s" << endl;
}

void testWordsCountSingleThread() {
    cout << "Start test word counter in single thread" << endl;

    auto text = FileReader::Read("data/words_count_text.txt");
    Yamr yamr = YamrConverter::ConvertToDummy(text);
    
    WordsCounterMapper mapper;
    WordsCounterReducer reducer;

    MapReduce mapReduce;

    auto start = StartTimer("Start single thread word counting");
    auto result = mapReduce(yamr, mapper, reducer);
    EndTimer("End single thread word counting", start);

    FileWriter::Write("data/words_count_result_single_thread.txt", result);

    cout << "End test word counter in single thread" << endl << endl;
}

void testWordsCount() {
    cout << "Start test word counter" << endl;

    auto text = FileReader::Read("data/words_count_text.txt");
    Yamr yamr;
    for (const auto& data : Splitter::Split('\n', text)) {
        yamr.push_back(YamrConverter::ConvertToDummy(data)[0]);
    }
    
    WordsCounterMapper mapper;
    WordsCounterReducer reducer;

    MapReduce mapReduce;

    auto start = StartTimer("Start multi thread word counting");
    auto result = mapReduce(yamr, mapper, reducer);
    EndTimer("End multi thread word counting", start);

    FileWriter::Write("data/words_count_result.txt", result);

    cout << "End test word counter" << endl << endl;
}

void testSort() {
    cout << "Start test sort" << endl;

    auto input = FileReader::Read("data/sort_data.txt");
    auto yamr = YamrConverter::ConvertToYamr(input);

    DummyMapper mapper;
    DummyReducer reducer;

    MapReduce mapReduce;

    auto start = StartTimer("Start sort");
    auto result = mapReduce(yamr, mapper, reducer);
    EndTimer("End sort", start);

    FileWriter::Write("data/sorted_data.txt", result);  

    cout << "End test sort" << endl << endl;
}

int main(int argc, const char * argv[]) {
    cout << "For test sort enter 1" << endl
              << "For test multithread word counter enter 2" << endl
              << "For test one thred word counter enter 3" << endl;

    int mode;
    cin >> mode;

    switch (mode) {
        case 1:
            testSort();
            break;
        case 2:
            testWordsCount();
            break;
        case 3:
            testWordsCountSingleThread();
            break;
        default:
            throw runtime_error("Incorrect mode");
    }
    
    return 0;
}
