#ifndef MapReduce_iface_h
#define MapReduce_iface_h

#include <vector>
#include <string>


using Yamr = std::vector<std::pair<std::string, std::string>>;
using YamrRow = std::pair<std::string, std::string>;

class IMapper {
public:
    virtual Yamr operator()(Yamr& input) = 0;
};

class IReducer {
public:
    virtual Yamr operator()(Yamr& input) = 0;
};

class DummyMapper final : public IMapper {
public:
    Yamr operator()(Yamr& input) override {
        return input;
    }
};

class DummyReducer final : public IReducer {
public:
    Yamr operator()(Yamr& input) override {
        return input;
    }
};

#endif
