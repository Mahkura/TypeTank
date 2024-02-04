#pragma once
#include <events/EventSink.hpp>
#include <optional>
#include <vector>

namespace tt
{

struct CoreLogic
{
    struct Tank
    {
        uint32_t id;
        uint32_t img_id;
        std::string name;
        Point pos;
        bool owned;
    };

    struct Camera
    {
        uint32_t id;
        uint32_t tank_id;
        Point pos;
    };

    EventSink* output_sink;
    EventQueue input_queue;

    std::optional<Camera> camera;
    std::vector<TankCreated> pending_tank_creations;
    std::vector<TankMoved> pending_tank_moves;

    std::vector<Tank> tanks;

    void register_queue(EventSink& sink);
    void execute();
    void update_logic();
};

} // namespace tt
