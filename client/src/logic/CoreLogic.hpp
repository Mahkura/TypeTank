#pragma once
#include <events/EventSink.hpp>

namespace tt
{

struct CoreLogic
{
    EventSink* output_sink;
    EventQueue input_queue;

    void register_queue(EventSink& sink);
    void execute();
};

} // namespace tt
