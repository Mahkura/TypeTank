#pragma once
#include <string>
#include <common/Units.hpp>
#include <cstdint>

namespace tt
{

struct RequiresImg
{
    std::string img_path;
    uint32_t id;
    Size16 pixel_size;
    Size world_size;
};

struct DrawImg
{
    uint32_t id;
    Point world_coordinate;
    uint16_t scale; // TODO: define
    uint16_t rotation; // TODO: define
    uint16_t layer;
};

struct CreateCamera
{
    uint32_t id;
    Point world_coordinate;
    Size world_size;
    // TODO: window location? in case we want multiple cameras within the window
};

struct MoveCamera
{
    uint32_t id;
    Point world_coordinate;
};

struct Present
{
};

struct RenderDone
{
};

} // namespace tt
