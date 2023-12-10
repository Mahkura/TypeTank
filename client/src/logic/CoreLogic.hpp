#pragma once
#include <events/EventSink.hpp>
#include <optional>

namespace tt
{

struct CoreLogic
{
    EventSink* output_sink;
    EventQueue input_queue;

    std::optional<uint32_t> camera;

    void register_queue(EventSink& sink);
    void execute();
};

} // namespace tt
