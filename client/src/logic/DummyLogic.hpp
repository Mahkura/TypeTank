#pragma once
#include <events/EventSink.hpp>

namespace tt
{
struct DummyLogic
{
    EventSink* output_sink;
    EventQueue input_queue;

    bool initialized = false;
    // TODO what is the play area?
    uint16_t x = 0;
    uint16_t y = 0;
    std::string tank_name;
    uint16_t vel_x = 1000;
    uint16_t target_x = 0;

    UnitId tank;

    void execute();

    void save_velocity(UnitMove);
    void move_tank(FrameStarted);
};
} // namespace tt
