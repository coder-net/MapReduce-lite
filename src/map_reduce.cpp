#include "map_reduce.h"
#include "parallel_sort.h"

#include <functional>
#include <future>
#include <algorithm>
#include <vector>


Yamr MapReduce::ChunkReduce(IReducer& reducer, size_t start_index, size_t last_index, Yamr& data) {
    Yamr tmp(data.begin() + start_index, data.begin() + last_index);
    return reducer(tmp);
}

Yamr MapReduce::ChunkMap(IMapper& mapper, size_t start_index, size_t last_index, Yamr& data) {
    Yamr tmp(data.begin() + start_index, data.begin() + last_index);
    Yamr result = mapper(tmp);
    std::sort(result.begin(), result.end()); // Тут же отсортируем. Тогда дальше быстрая сортировка будет происходить быстрее.
    return result; // Вернем обработанный чанк после большого мэпа.
}

Yamr MapReduce::Reduce(IReducer& reducer, Yamr& mapped_data, size_t threads_number) {
    std::vector<std::pair<size_t, size_t>> same_key_ranges;
    std::string prev_key = mapped_data[0].first;
    for (size_t i = 0; i < mapped_data.size(); ++i) {
        if (mapped_data[i].first != prev_key) {
            if (same_key_ranges.empty()) {
                same_key_ranges.push_back(std::make_pair(0, i));
            } else {
                same_key_ranges.push_back(std::make_pair((same_key_ranges.end() - 1)->second, i));
            }
            prev_key = mapped_data[i].first;
        }
        
    }
    same_key_ranges.push_back(std::make_pair((same_key_ranges.end() - 1)->second, mapped_data.size()));
    TasksQueue<Yamr> reduce_planner(threads_number);
    std::vector<std::future<Yamr>> reduce_chunk_future;
    for (auto iter = same_key_ranges.begin(); iter != same_key_ranges.end(); ++iter) {
        auto chunk_reduce_task = std::bind(&MapReduce::ChunkReduce, this, std::ref(reducer), (*iter).first, (*iter).second, std::ref(mapped_data));
        reduce_chunk_future.emplace_back(reduce_planner.Submit(chunk_reduce_task));
    }
    reduce_planner.Shutdown();
    Yamr result;
    for (auto& reduce_future : reduce_chunk_future) {
        for (auto& reduce_chunk_result : reduce_future.get()) {
            result.emplace_back(reduce_chunk_result);
        }
    }
    return result;
}

Yamr MapReduce::Partition(Yamr& mapped_data, size_t threads_number) {
    TasksQueue<Yamr> planner(threads_number);
    SortTask<std::pair<std::string, std::string>> sorting(mapped_data, planner);
    std::sort(mapped_data.begin(), mapped_data.end());
    return mapped_data; // Already sorted data.
}

Yamr MapReduce::Map(Yamr& data, IMapper& mapper, std::vector<std::vector<size_t>>& indices, size_t threads_number) {
    TasksQueue<Yamr> map_planner(threads_number); // Task scheduller implemented by thread pool.
    std::vector<std::future<Yamr>> map_chunk_future; // Vector of pointers to result.
    for (size_t i = 0; i < threads_number; ++i) {
        size_t start_index = indices[i][0];
        size_t chunk_size = indices[i][1] - indices[i][0];
        auto chunk_map_task = std::bind(&MapReduce::ChunkMap, this, std::ref(mapper), start_index, start_index + chunk_size, std::ref(data)); // Создали задачу для мэпа для чанка.
        map_chunk_future.push_back(map_planner.Submit(chunk_map_task)); // Закинули в пул.
    } // Results будет содержать остортированные куски данных размером с чанк.
    
    map_planner.Shutdown();
    std::vector<std::thread> filling_threads;
    Yamr result;
    for (auto& map_future : map_chunk_future) {
        for (auto& map_chunk_result : map_future.get()) {
            result.emplace_back(map_chunk_result);
        }
    }
    return result;
}

void MapReduce::Decompose(Yamr& data, size_t threads_num, std::vector<std::vector<size_t>>& indices) {
    indices.resize(threads_num);
    size_t thread_number = 0;
    size_t block_size = data.size() / threads_num;
    for (size_t i = 0; i + block_size <= data.size() && thread_number < threads_num; i += block_size) {
        indices[thread_number].push_back(i);
        indices[thread_number].push_back(i + block_size);
        ++thread_number;
    }
    if (!indices[threads_num - 1].size()) {
        indices[threads_num - 1].push_back(indices[threads_num - 2][1]);
        indices[threads_num - 1].push_back(data.size());
    } else if (indices[threads_num - 1][1] != data.size()) {
        indices[threads_num - 1][1] = data.size();
    }
}

Yamr MapReduce::operator()(Yamr& data, IMapper& mapper, IReducer& reducer, size_t threads_number) {
    std::vector<std::vector<size_t>> indices;
    indices.resize(threads_number);
    if (!threads_number) {
        threads_number = std::thread::hardware_concurrency();
        if (!threads_number) {
            throw std::logic_error("Incompatible number of threads(0).\n");
        }
    }
    Decompose(data, threads_number, indices);
    for (size_t ind = 0; ind < indices.size(); ++ind) {
        if (indices[ind][0] == 0 && indices[ind][1] == 0) {
            indices.erase(indices.begin() + ind);
            --threads_number;
        }
    }
    auto mapped = Map(data, mapper, indices, threads_number);
    auto partitioned = Partition(mapped, threads_number);
    auto reduced = Reduce(reducer, partitioned, threads_number);
    return reduced;
}
