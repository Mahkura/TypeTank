#pragma once
#include <cstdint>
#include <string>

namespace tt
{
struct UnitId
{
    uint8_t unit_id;
    std::string name;
};

struct UnitMove
{
    enum Type : uint8_t
    {
        Meters
    };
    Type type;
    uint8_t unit_id;
    uint16_t move_amount;
};

} // namespace tt
