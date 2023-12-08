#include "SDL.h"
#include "SDL_image.h"
#include <graphics/Graphics.hpp>

// runs through different graphics funcionality
// TODO: manage "graphics" lifetime to be able to restart it?
struct RunSystem
{
    struct PutEvent
    {
        tt::Event event;
    };

    struct Pause
    {
        uint32_t pause_time_ms;
    };

    using Action = std::variant<PutEvent, Pause>;

    struct Run
    {
        std::string name;
        std::string expectation;
        std::deque<Action> actions;
    };

    tt::EventSink* output_sink;
    tt::EventQueue input_queue;

    bool first_run = true;
    std::queue<Run> feature_runs;

    void register_queue(tt::EventSink& sink)
    {
        sink.register_queue<tt::RenderDone>(&input_queue);
    }

    void init()
    {
        feature_runs.push(
        {
            .name = "image can be rotated",
            .expectation = "image is rotating around its center",
            .actions = {
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 1,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 3000, .height = 5400 }
                }},
                PutEvent { .event = tt::CreateCamera {
                    .id = 1,
                    .world_coordinate = { .x = 0, .y = 0 },
                    .world_size = { .width = 20'000, .height = 20'000 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 200 },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                    .rotation = 3000,
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 200 },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                    .rotation = 6000,
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 200 },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                    .rotation = 9000,
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 200 },
            }
        });
        feature_runs.push(
        {
            .name = "Draw multiple images",
            .expectation = "2 different sized images are drawn on screen side by side",
            .actions = {
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 1,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 3000, .height = 5400 }
                }},
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 2,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 1500, .height = 2700 }
                }},
                PutEvent { .event = tt::CreateCamera {
                    .id = 1,
                    .world_coordinate = { .x = 0, .y = 0 },
                    .world_size = { .width = 20'000, .height = 20'000 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2000, .y = 0 },
                }},
                PutEvent { .event = tt::Present {} },
            }
        });
        feature_runs.push(
        {
            .name = "Draw images on top of each other",
            .expectation = "2 different sized images are drawn, smaller one on top of the bigger one",
            .actions = {
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 1,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 3000, .height = 5400 }
                }},
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg", // TODO: change to a more visible image
                    .id = 2,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 1500, .height = 2700 }
                }},
                PutEvent { .event = tt::CreateCamera {
                    .id = 1,
                    .world_coordinate = { .x = 0, .y = 0 },
                    .world_size = { .width = 20'000, .height = 20'000 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 0, .y = 0 },
                    .layer = 0,
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = 0, .y = 2900 },
                    .layer = 1,
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 300 },
                // also send events in different order to verify layering still works
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = 0, .y = 2900 },
                    .layer = 1,
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 0, .y = 0 },
                    .layer = 0,
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 300 },
            }
        });

        feature_runs.push(
        {
            .name = "Image is only drawn when explicitly asked to be drawn",
            .expectation = "2 images are drawn on screen, left one disappears",
            .actions = {
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 1,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 3000, .height = 5400 }
                }},
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 2,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 1500, .height = 2700 }
                }},
                PutEvent { .event = tt::CreateCamera {
                    .id = 1,
                    .world_coordinate = { .x = 0, .y = 0 },
                    .world_size = { .width = 20'000, .height = 20'000 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2000, .y = 0 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 500 },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 500 },
            }
        });

        feature_runs.push(
        {
            .name = "Camera can be moved",
            .expectation = "2 images are drawn, both move",
            .actions = {
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 1,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 3000, .height = 5400 }
                }},
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 2,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 1500, .height = 2700 }
                }},
                PutEvent { .event = tt::CreateCamera {
                    .id = 1,
                    .world_coordinate = { .x = 0, .y = 0 },
                    .world_size = { .width = 20'000, .height = 20'000 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2000, .y = 0 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 200 },
                PutEvent { .event = tt::MoveCamera{ .id = 1, .world_coordinate = { .x = 500, .y = 0 } } },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2000, .y = 0 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 200 },
                PutEvent { .event = tt::MoveCamera{ .id = 1, .world_coordinate = { .x = 800, .y = 400 } } },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2000, .y = 0 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 200 },
                PutEvent { .event = tt::MoveCamera{ .id = 1, .world_coordinate = { .x = 1000, .y = 900 } } },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2000, .y = 0 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 200 },
            }
        });

        feature_runs.push(
        {
            .name = "Images can be drawn to different locations",
            .expectation = "2 images are drawn, left one is moving",
            .actions = {
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 1,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 3000, .height = 5400 }
                }},
                PutEvent { .event = tt::RequiresImg {
                    .img_path = "tank.svg",
                    .id = 2,
                    .pixel_size = { .width = 20, .height = 50 }, 
                    .world_size = { .width = 1500, .height = 2700 }
                }},
                PutEvent { .event = tt::CreateCamera {
                    .id = 1,
                    .world_coordinate = { .x = 0, .y = 0 },
                    .world_size = { .width = 20'000, .height = 20'000 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2000, .y = 0 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 300 },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2500, .y = 500 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 300 },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2500, .y = 800 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 300 },
                PutEvent { .event = tt::DrawImg {
                    .id = 1,
                    .world_coordinate = { .x = 2000, .y = 0 },
                }},
                PutEvent { .event = tt::DrawImg {
                    .id = 2,
                    .world_coordinate = { .x = -2100, .y = 400 },
                }},
                PutEvent { .event = tt::Present {} },
                Pause { .pause_time_ms = 300 },
            }
        });
        // TODO: init feature runs here as queues of events
    }

    void show_start_message(const Run& run)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_INFORMATION,
            run.name.c_str(),
            run.expectation.c_str(),
            0);
    }

    void show_end_message(const Run& run)
    {
        SDL_MessageBoxButtonData btns[2] =  {
            {
                .flags = 0,
                .buttonid = 1,
                .text = "Success"
            },
            {
                .flags = 0,
                .buttonid = 2,
                .text = "Failure"
            },
        };
        auto msg_box_data = SDL_MessageBoxData {
            .flags = SDL_MESSAGEBOX_INFORMATION,
            .window = 0,
            .title = run.name.c_str(),
            .message = run.expectation.c_str(),
            .numbuttons = 2,
            .buttons = btns,
        };

        int btn_id = 0;
        SDL_ShowMessageBox(
            &msg_box_data,
            &btn_id);

        // TODO what to do in case of failure? Quit? mark run failed?
    }

    void execute()
    {
        if (!input_queue.empty())
        {
            std::visit(
                [&](auto &&e)
                {
                    using T = std::decay_t<decltype(e)>;
                    if constexpr (std::is_same_v<T, tt::RenderDone>)
                    {
                        SDL_Delay(16);
                    }
                },
                input_queue.front());

            input_queue.pop();
        }

        // run through features based on the config above
        if (feature_runs.empty())
            return;
        
        auto& run = feature_runs.front();
        if (first_run)
        {
            first_run = false;
            show_start_message(run);
        }

        if (run.actions.empty())
        {
            show_end_message(run);
            feature_runs.pop();

            if (!feature_runs.empty())
               show_start_message(feature_runs.front());

            return;
        }
        
        auto& action = run.actions.front();
        std::visit(
            [&](auto &&e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr(std::is_same_v<T, PutEvent>)
                {
                    output_sink->put_event(e.event);
                }
                else if constexpr(std::is_same_v<T, Pause>)
                {
                    SDL_Delay(e.pause_time_ms);
                }
            },
            action);

        run.actions.pop_front();
    }
};

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(0);

    auto window = SDL_CreateWindow(
        "TypeTank",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        300, 300,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

    auto event_sink = tt::EventSink{};
    auto graphics = tt::Graphics{
        .output_sink = &event_sink,
        .renderer = renderer,
        .window_resolution = { .width = 300, .height = 300 } // TODO: what to do when window is resized?
    };
    graphics.register_queue(event_sink);

    auto run_system = RunSystem{ .output_sink = &event_sink };
    run_system.register_queue(event_sink);
    run_system.init();

    SDL_Event e;
    bool running = !run_system.feature_runs.empty();
    while (running)
    {
        while (SDL_PollEvent(/*out*/ &e) != 0)
        {
            if (e.type == SDL_QUIT)
                running = false;
        }

        run_system.execute();
        graphics.execute();

        running = !run_system.feature_runs.empty();
    }

    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
 
    return 0;
}