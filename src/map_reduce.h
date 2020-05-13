#ifndef MapReduce_map_reduce_h
#define MapReduce_map_reduce_h

#include "iface.h"


class MapReduce {
public:
    Yamr operator()(Yamr& data, IMapper& mapper, IReducer& reducer, size_t threads_number = 0);

private:
    Yamr ChunkReduce(IReducer& reducer, size_t start_index, size_t last_index, Yamr& data);
    
    Yamr ChunkMap(IMapper& mapper, size_t start_index, size_t last_index, Yamr& data);
    
    Yamr Reduce(IReducer& reducer, Yamr& mapped_data, size_t threads_number);
    
    Yamr Map(Yamr& data, IMapper& mapper, std::vector<std::vector<size_t>>& indices, size_t threads_number);

    Yamr Partition(Yamr& mapped_data, size_t threads_number);
    
    void Decompose(Yamr& data, size_t threads_num, std::vector<std::vector<size_t>>& indices);
};


#endif
