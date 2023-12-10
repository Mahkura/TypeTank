#pragma once
#include <common/Units.hpp>
#include <string>

namespace tt
{
struct TankCreated
{
    enum Flags : unsigned
    {
        MyTank = 0x1,
    };
    std::string name;
    uint32_t flags;
    PointU16 pos_dm;
    Size16 size_dm;
};

struct TankMoved
{
    std::string name;
    PointU16 pos_dm;
};
} // namespace tt
