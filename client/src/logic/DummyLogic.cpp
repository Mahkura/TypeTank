#include <logic/DummyLogic.hpp>
#include <type_traits>

namespace tt
{
    
void DummyLogic::execute()
{
    if (!initialized)
    {
        initialized = true;
        tank_name = "tanky";
        output_sink->put_event(TankCreated{
            .name = tank_name,
            .pos_x = x,
            .pos_y = y,
        });
        return;
    }

    while (!input_queue.empty())
    {
        std::visit(
            [&](auto &&e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr(std::is_same_v<T, UnitId>)
                {
                    tank = e;
                    if (tank.name != tank_name)
                        tank = {};
                }
                else if constexpr(std::is_same_v<T, UnitMove>)
                    save_velocity(e);
                else if constexpr(std::is_same_v<T, FrameStarted>)
                    move_tank(e);
            },
            input_queue.front());
        
        input_queue.pop();
    }
}

void DummyLogic::save_velocity(UnitMove e)
{
    if (e.unit_id == tank.unit_id)
        target_x = x + (e.move_amount * 100);
}

void DummyLogic::move_tank(FrameStarted e)
{
    if (x == target_x)
        return;

    auto move_x = vel_x * e.usec_since_last_frame / 1'000'000;
    x += move_x;
    if (x > target_x)
        x = target_x;

    output_sink->put_event(TankMoved{
        .name = tank_name,
        .pos_x = uint16_t(x / 100),
        .pos_y = uint16_t(y / 100),
    });
}

} // namespace tt
