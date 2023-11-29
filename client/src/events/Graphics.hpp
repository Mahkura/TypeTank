#pragma once
#include <string>
#include <cstdint>

namespace tt
{

struct SetPixelRatio
{
    uint16_t min_units_per_dimension;
};

struct RequiresImg
{
    std::string img_path;
    uint32_t id;
    uint16_t width;
    uint16_t height;
};

struct DrawImg
{
    uint32_t id;
    uint16_t x;
    uint16_t y;
    uint16_t scale; // TODO: define
    uint16_t rotation; // TODO: define
};

struct Present
{
};

} // namespace tt
