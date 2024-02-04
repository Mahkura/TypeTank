#include <logic/CoreLogic.hpp>
#include <ranges>

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
                    // TODO: load some foliage to draw?
                    // TODO: later on, load tiled map ground textures?
                }
                else if constexpr(std::is_same_v<T, TankCreated>)
                {
                    pending_tank_creations.push_back(e);
                }
                else if constexpr(std::is_same_v<T, TankMoved>)
                {
                    pending_tank_moves.push_back(e);
                }
                else if constexpr(std::is_same_v<T, FrameStarted>)
                {
                    update_logic();
                }
            },
            input_queue.front());

        input_queue.pop();
    }
}

void CoreLogic::update_logic()
{
    // create new tanks
    auto id = 1u;
    // TODO: existing tank
    for (const auto& e : pending_tank_creations)
    {
        tanks.push_back(Tank{
            .id = id,
            .img_id = id,
            .name = e.name,
            .pos = Point{.x = e.pos_dm.x * 100, .y = e.pos_dm.y * 100},
            .owned = ((e.flags & tt::TankCreated::MyTank) == tt::TankCreated::MyTank),
        });
        ++id;

        const auto& t = tanks.back();
        if (t.owned && !camera.has_value())
        {
            camera = Camera{.id = 1, .tank_id = t.id, .pos = t.pos};
            output_sink->put_event(CreateCamera{
                .id = camera->id,
                .world_coordinate = camera->pos,
                .world_size = {.width = 40'000, .height = 40'000}});
        }
    }

    // TODO: stop if camera does not exist (clear pending creations)

    // loading images separately since we want CreateCamera to happen before any RequiresImg
    for (const auto& e : pending_tank_creations)
    {
        auto it_tank = std::ranges::find_if(
            tanks, 
            [&](auto& t) { return t.name == e.name; });
        if (it_tank == std::end(tanks))
        {
            // TODO: error handling
            continue;
        }
        output_sink->put_event(RequiresImg{
            .img_path = "tank.svg",
            .id = it_tank->img_id,
            .world_size = {.width = e.size_dm.width * 100u, .height = e.size_dm.height * 100u},
        });
    }
    pending_tank_creations.clear();

    // move tanks
    for (const auto& e : pending_tank_moves)
    {
        auto it_tank = std::ranges::find_if(
            tanks, 
            [&](auto& t) { return t.name == e.name; });
        if (it_tank == std::end(tanks))
        {
            // TODO: error handling
            continue;
        }

        it_tank->pos = { .x = e.pos_dm.x * 100, .y = e.pos_dm.y * 100 };
    }
    pending_tank_moves.clear();

    // move camera
    auto it_tank = std::ranges::find_if(
        tanks, 
        [&](auto& t) { return t.id == camera->tank_id; });
    if (it_tank == std::end(tanks))
    {
        // TODO: error handling
    }
    else
    {
        if (it_tank->pos.x != camera->pos.x ||
            it_tank->pos.y != camera->pos.y)
        {
            camera->pos = it_tank->pos;
            output_sink->put_event(MoveCamera{
                .id = camera->id,
                .world_coordinate = camera->pos,
            });
        }
    }

    // draw tanks
    for (auto& t : tanks)
    {
        output_sink->put_event(DrawImg{
            .id = t.id,
            .world_coordinate = { .x = t.pos.x, .y = t.pos.y },
            .rotation = 0,
            .layer = 10,
        });
    }
}
    
} // namespace tt
