#pragma once
#include <events/EventSink.hpp>

namespace tt
{
struct FileFollower
{
    EventSink* output_sink;
    std::string file_to_follow;
    uint64_t file_last_modified = 0;
    uint64_t file_last_hash = 0;

    auto create_files() -> bool;
    void execute();
};

} // namespace tt
