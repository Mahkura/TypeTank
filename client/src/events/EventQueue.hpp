#pragma once
#include <variant>
#include <queue>

#include "FrameStarted.hpp"
#include "UnitId.hpp"
#include "Tank.hpp"
#include "ScriptChanged.hpp"
#include "Map.hpp"
#include "Graphics.hpp"

namespace tt
{
using Event = std::variant<
    FrameStarted,
    UnitId,
    UnitMove,
    TankCreated,
    TankMoved,
    Map,
    SetPixelRatio,
    RequiresImg,
    DrawImg,
    Present,
    ScriptChanged>;
using EventQueue = std::queue<Event>;

} // namespace tt
