#pragma once
#include <cstdint>
#include <string>

namespace tt
{
struct TankCreated
{
    enum Flags
    {
        MyTank = 0x1,
    };
    std::string name;
    uint32_t flags;
    uint16_t pos_x;
    uint16_t pos_y;
    uint16_t width;
    uint16_t height;
};

struct TankMoved
{
    std::string name;
    uint16_t pos_x;
    uint16_t pos_y;
};
} // namespace tt
