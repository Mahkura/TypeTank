#include "Connection.hpp"

namespace tt
{

auto Connection::init() -> int
{
    socketset = SDLNet_AllocSocketSet(1);
    return 0;
}

void Connection::close()
{
    if (socket != NULL)
        SDLNet_TCP_Close(socket);

    socket = NULL;
    SDLNet_FreeSocketSet(socketset);
}

void Connection::register_input(EventSink& sink)
{
    sink.register_queue<Connect>(&input_queue);
}

void Connection::execute()
{
    while (!input_queue.empty())
    {
        std::visit(
            [&](auto &&e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr(std::is_same_v<T, Connect>)
                {
                    // TODO: handle multiple connection attemps
                    auto addr = IPaddress{};
                    SDLNet_ResolveHost(&addr, e.url.c_str(), e.port);
                    if (addr.host == INADDR_NONE)
                    {
                        output_sink->put_event(Disconnected{ .url = e.url });
                        return;
                    }

                    socket = SDLNet_TCP_Open(&addr);
                    if (socket == NULL)
                    {
                        output_sink->put_event(Disconnected{ .url = e.url });
                        return;
                    }

                    SDLNet_TCP_AddSocket(socketset, socket);
                    output_sink->put_event(Connected{ .url = e.url });
                }
                else if constexpr(std::is_same_v<T, SendData>)
                {
                    SDLNet_TCP_Send(socket, e.buffer, e.len);
                    // TODO error handling
                }
            },
            input_queue.front());

        input_queue.pop();
    }

    // check for input from socket
    if (socket != NULL)
    {
        if (SDLNet_CheckSockets(socketset, 0) > 0)
        {
            auto received = DataReceived{};
            const auto receive_size = SDLNet_TCP_Recv(socket, received.buffer, sizeof(received.buffer)); 
            // TODO error handling
            if (receive_size > 0)
            {
                received.len = receive_size;
                output_sink->put_event(received);
            }
        }
    }
}
    
} // namespace tt
