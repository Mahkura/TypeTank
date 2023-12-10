#include <logic/CoreLogic.hpp>

namespace tt
{

void CoreLogic::register_queue(EventSink& sink)
{
    sink.register_queue<
        FrameStarted,
        Map,
        TankCreated,
        TankMoved>(
        /*inout*/&input_queue);
}

void CoreLogic::execute()
{
    while (!input_queue.empty())
    {
        std::visit(
            [&](auto &&e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr(std::is_same_v<T, Map>)
                {
                    // TODO: set view scaling? (ie. somehow manage different window pixel sizes)
                    // TODO: load typeface?
                    // TODO: later on, load tiled map ground textures?
                }
                else if constexpr(std::is_same_v<T, TankCreated>)
                {
                    // TODO: load Tank image if not loaded
                    if ((e.flags & tt::TankCreated::MyTank) == tt::TankCreated::MyTank)
                    {
                        if (!camera.has_value())
                        {
                            camera = 1;
                            output_sink->put_event(CreateCamera{
                                .id = 1,
                                .world_coordinate = {.x = e.pos_dm.x * 100, .y = e.pos_dm.y * 100},
                                .world_size = {.width = 40'000, .height = 40'000}});
                        }
                    }
                }
                else if constexpr(std::is_same_v<T, TankMoved>)
                {
                    // TODO: check that tank exists
                    // TODO: update internal state
                    output_sink->put_event(MoveCamera{
                        .id = camera.value(),
                        .world_coordinate = { .x = e.pos_dm.x * 100, .y = e.pos_dm.y * 100},
                    });
                }
                else if constexpr(std::is_same_v<T, FrameStarted>)
                {
                    // TODO: draw everything
                }
            },
            input_queue.front());

        input_queue.pop();
    }
}
    
} // namespace tt
