#pragma once
#include <events/EventSink.hpp>
#include <string>

namespace tt
{

struct ScriptParser
{
    EventSink* output_sink;
    EventQueue input_queue;

    void register_input(EventSink& sink);
    void execute();
};

} // namespace tt
