#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_net.h"

#include <logic/DummyLogic.hpp>
#include <script/ScriptParser.hpp>
#include <script/FileFollower.hpp>
#include <net/Connection.hpp>
#include <net/CommParser.hpp>
#include <graphics/Graphics.hpp>

int main(int argc, char** argv)
{
    // TODO: check return values
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(0);
    TTF_Init();
    SDLNet_Init();

    auto window = SDL_CreateWindow(
        "TypeTank",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        300, 300,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

    auto event_sink = tt::EventSink{};
    auto file_follower = tt::FileFollower{ .output_sink = &event_sink };
    if (!file_follower.create_files())
        return -1;

    auto graphics = tt::Graphics{
        .output_sink = &event_sink,
        .renderer = renderer,
        .window_resolution = { .width = 300, .height = 300 }
    };
    graphics.register_queue(event_sink);

    auto script_parser = tt::ScriptParser{ .output_sink = &event_sink };
    script_parser.register_input(event_sink);

    auto connection = tt::Connection{.output_sink = &event_sink };
    connection.register_input(event_sink);
    connection.init();

    auto comms_parser = tt::CommParser{ .output_sink = &event_sink };
    comms_parser.register_input(event_sink);

    auto graphics_queue = tt::EventQueue{};
    event_sink.register_queue<tt::TankMoved>(/*inout*/&graphics_queue);

    event_sink.put_event(tt::Connect{
        .url = "127.0.0.1",
        .port = 8080,
    });

    SDL_Event e;
    bool running = true;
    while (running)
    {
        while (SDL_PollEvent(/*out*/ &e) != 0)
        {
            if (e.type == SDL_QUIT)
                running = false;
        }

        // TODO: run these for as long as there are events to process
        file_follower.execute();
        script_parser.execute();
        connection.execute();
        comms_parser.execute();
        graphics.execute();

        // TODO: move render logic to own file
        while (!graphics_queue.empty())
        {
            std::visit(
                [&](auto &&e)
                {
                    using T = std::decay_t<decltype(e)>;
                    if constexpr (std::is_same_v<T, tt::RenderDone>)
                    {
                        SDL_Delay(16);

                        event_sink.put_event(tt::FrameStarted{
                            .usec_since_last_frame = 16000 // TODO get this from SDL
                        });
                    }
                },
                graphics_queue.front());

            graphics_queue.pop();
        }
    }

    SDL_DestroyWindow(window);

    connection.close();

    SDLNet_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}