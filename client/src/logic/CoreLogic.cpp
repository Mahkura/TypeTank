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
                }
                else if constexpr(std::is_same_v<T, TankMoved>)
                {
                    // TODO: update internal state
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
