#include "examples/word_counter.h"
#include "src/map_reduce.h"
#include "src/utils.h"

#include <iostream>
#include <string>

using namespace std;

std::ostream& operator<<(std::ostream& out, const Yamr& yamr) {
    for (const auto& p : yamr) {
        out << p.first << " " << p.second << std::endl;
    }
    return out;
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

    auto result = mapReduce(yamr, mapper, reducer);

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

    auto result = mapReduce(yamr, mapper, reducer);

    FileWriter::Write("data/sorted_data.txt", result);  

    cout << "End test sort" << endl << endl;
}

int main(int argc, const char * argv[]) {
    testWordsCount();
    testSort();
    
    return 0;
}
