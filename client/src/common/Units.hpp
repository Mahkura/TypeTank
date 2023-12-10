#pragma once
#include <cstdint>

namespace tt
{

struct Point
{
    int32_t x;
    int32_t y;
};

struct Point16
{
    int16_t x;
    int16_t y;
};

struct PointU16
{
    uint16_t x;
    uint16_t y;
};

struct Size
{
    uint32_t width;
    uint32_t height;
};

struct Size16
{
    uint16_t width;
    uint16_t height;
};

} // namespace tt
