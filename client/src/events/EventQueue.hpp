#pragma once
#include <variant>
#include <queue>

#include "FrameStarted.hpp"
#include "UnitId.hpp"

namespace tt
{
using Event = std::variant<FrameStarted, UnitId, UnitMove>;
using EventQueue = std::queue<Event>;

} // namespace tt
