#pragma once
#include <events/EventSink.hpp>
#include <common/Units.hpp>

namespace tt
{
struct DummyLogic
{
    EventSink* output_sink;
    EventQueue input_queue;

    bool initialized = false;
    // TODO what is the play area?
    PointU16 tank_pos_dm = { .x = 100, .y = 100 };
    std::string tank_name;
    int16_t vel_y = 1000;
    PointU16 target_pos_dm;

    UnitId tank;

    void execute();

    void save_velocity(UnitMove);
    void move_tank(FrameStarted);
};
} // namespace tt
