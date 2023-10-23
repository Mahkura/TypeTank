#pragma once
#include <variant>
#include <queue>

#include "FrameStarted.hpp"
#include "UnitId.hpp"
#include "Tank.hpp"

namespace tt
{
using Event = std::variant<FrameStarted, UnitId, UnitMove, TankCreated, TankMoved>;
using EventQueue = std::queue<Event>;

} // namespace tt
