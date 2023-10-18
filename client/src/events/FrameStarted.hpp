#pragma once
#include <cstdint>

namespace tt
{
struct FrameStarted
{
    uint32_t usec_since_last_frame;
};
} // namespace tt
