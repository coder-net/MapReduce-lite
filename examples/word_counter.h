#ifndef MapReduce_word_counter_h
#define MapReduce_word_counter_h

#include "../src/iface.h"


class WordsCounterMapper : public IMapper {
private:
    void MapData(Yamr& mapped_data, const std::string& text) const;
    
public:
    Yamr operator () (Yamr& input) override;
};

class WordsCounterReducer : public IReducer {
public:
    Yamr operator () (Yamr& mapped_data) override;
};

#endif
