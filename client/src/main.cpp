#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_net.h"

#include <logic/DummyLogic.hpp>
#include <script/ScriptParser.hpp>

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
        SDL_WINDOW_SHOWN);
    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

    auto img = IMG_LoadTexture(renderer, "tank.svg");
    if (!img)
        return -1;

    auto eventSink = tt::EventSink{};
    auto scriptParser = tt::ScriptParser{ .output_sink = &eventSink };
    scriptParser.script_content = R"(
            unit tanky {
                move 100m
            }
        )";
    auto logic = tt::DummyLogic{ .output_sink = &eventSink };
    eventSink.register_queue<tt::UnitId, tt::UnitMove, tt::FrameStarted>(
        /*inout*/&logic.input_queue);

    auto graphics_queue = tt::EventQueue{};
    eventSink.register_queue<tt::TankMoved>(/*inout*/&graphics_queue);

    auto tank_img_rect = SDL_Rect{
        .x = 0,
        .y = 0,
        .w = 30,
        .h = 54,
    };

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
        scriptParser.execute();
        logic.execute();

        // move render logic to own file
        while (!graphics_queue.empty())
        {
            std::visit(
                [&](auto &&e)
                {
                    using T = std::decay_t<decltype(e)>;
                    if constexpr (std::is_same_v<T, tt::TankMoved>)
                    {
                        tank_img_rect.x = e.pos_x;
                        tank_img_rect.y = e.pos_y;
                    }
                },
                graphics_queue.front());

            graphics_queue.pop();
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, img, nullptr, &tank_img_rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);

        eventSink.put_event(tt::FrameStarted{
            .usec_since_last_frame = 16000 // TODO get this from SDL
        });
    }

    SDL_DestroyTexture(img);
    SDL_DestroyWindow(window);

    SDLNet_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}