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

        output_sink->put_event(Map{
            .width_m = 1000,
            .height_m = 1000,
        });

        output_sink->put_event(TankCreated{
            .name = tank_name,
            .flags = TankCreated::MyTank,
            .pos_dm = tank_pos_dm,
            .size_dm = { .width = 30, .height = 55 },
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
    {
        target_pos_dm = {
            .x = tank_pos_dm.x,
            .y = (uint16_t)(tank_pos_dm.y + (e.move_amount * 10)),
        };
    }
}

void DummyLogic::move_tank(FrameStarted e)
{
    if (tank_pos_dm.y == target_pos_dm.y)
        return;

    auto move_y = vel_y * e.usec_since_last_frame / 1'000'000;
    tank_pos_dm.y += move_y;
    if (tank_pos_dm.y > target_pos_dm.y)
        tank_pos_dm.y = target_pos_dm.y;

    output_sink->put_event(TankMoved{
        .name = tank_name,
        .pos_dm = tank_pos_dm,
    });
}

} // namespace tt
