#pragma once
#include <cstdint>
#include <string>

namespace tt
{
struct TankCreated
{
    std::string name;
    uint16_t pos_x;
    uint16_t pos_y;
};

struct TankMoved
{
    std::string name;
    uint16_t pos_x;
    uint16_t pos_y;
};
} // namespace tt
