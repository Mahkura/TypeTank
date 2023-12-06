#pragma once
#include <events/EventSink.hpp>

namespace tt
{

struct CommParser
{
    EventSink* output_sink;
    EventQueue input_queue;

    size_t expected_buffer_size = 0;
    std::string buffer;

    void register_input(EventSink& sink);
    void execute();
};

} // namespace tt
