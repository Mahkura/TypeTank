#pragma once
#include <events/EventSink.hpp>

namespace tt
{

struct CommParser
{
    EventSink* output_sink;
    EventQueue input_queue;

    std::string buffer;

    void register_input(EventSink& sink);
    void execute();
};

} // namespace tt
