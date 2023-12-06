#pragma once
#include <events/EventSink.hpp>
#include "SDL_net.h"

namespace tt
{
struct Connection
{
    EventSink* output_sink;
    EventQueue input_queue;

    TCPsocket socket;
    SDLNet_SocketSet socketset;

    void register_input(EventSink& sink);
    auto init() -> int;
    void close();
    void execute();
};
} // namespace tt

