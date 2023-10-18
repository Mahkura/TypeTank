#pragma once
#include <events/EventSink.hpp>
#include <string>

namespace tt
{

struct ScriptParser
{
    EventSink* output_sink;
    std::string script_content;

    void execute();
};

} // namespace tt
