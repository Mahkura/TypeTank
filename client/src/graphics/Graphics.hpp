#pragma once
#include <events/EventSink.hpp>
#include "SDL.h"

namespace tt
{

// Deals with a "world" with origin in the center (as opposed to screen coordinates, where origin is top left)
// Translates world coordinates to screen coordinates
struct Graphics
{
    struct Texture
    {
        uint32_t id;
        std::string path;
        SDL_Texture* texture;
    };

    struct Img
    {
        uint32_t id;
        uint32_t texture_id;
        Size world_size;
    };

    struct ScreenSpaceImg
    {
        uint32_t texture_id;
        SDL_Rect coordinates;
        uint16_t layer;
        float angle;
    };

    EventSink* output_sink;
    EventQueue input_queue;

    SDL_Renderer* renderer;
    Size window_resolution;

    std::vector<Texture> loaded_textures;
    std::vector<Img> loaded_images;
    std::vector<DrawImg> images_to_be_drawn;

    CreateCamera camera;

    std::vector<ScreenSpaceImg> buffer_ss_images;

    void register_queue(EventSink& sink);
    void execute();

    void draw_images();
};
    
} // namespace tt
