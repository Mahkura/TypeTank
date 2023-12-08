#pragma once
#include <variant>
#include <queue>

#include "FrameStarted.hpp"
#include "UnitId.hpp"
#include "Tank.hpp"
#include "ScriptChanged.hpp"
#include "Map.hpp"
#include "Graphics.hpp"
#include "Communication.hpp"

namespace tt
{
using Event = std::variant<
    Connect,
    Connected,
    Disconnected,
    DataReceived,
    SendData,
    FrameStarted,
    UnitId,
    UnitMove,
    TankCreated,
    TankMoved,
    Map,
    RequiresImg,
    DrawImg,
    Present,
    RenderDone,
    CreateCamera,
    MoveCamera,
    ScriptChanged>;
using EventQueue = std::queue<Event>;

} // namespace tt
