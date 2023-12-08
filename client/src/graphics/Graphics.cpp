#include "Graphics.hpp"
#include "SDL_image.h"

namespace tt
{

void Graphics::register_queue(EventSink& sink)
{
    sink.register_queue<
        RequiresImg,
        DrawImg,
        CreateCamera,
        MoveCamera,
        Present>(&input_queue);
}

void Graphics::execute()
{
    if (input_queue.empty())
        return;

    while (!input_queue.empty())
    {
        std::visit(
            [&](auto &&e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr(std::is_same_v<T, RequiresImg>)
                {
                    // TODO: first check if image is already loaded
                    auto t = IMG_LoadTexture(renderer, e.img_path.c_str());
                    // TODO: IMG_LoadSizedSVG_RW(); to load with requested pixel size
                    // TODO: error handling
                    auto texture_id = 0;
                    if (!loaded_textures.empty())
                        texture_id = loaded_textures.back().id + 1;
                    
                    auto& texture = loaded_textures.emplace_back();
                    texture.id = texture_id;
                    texture.path = e.img_path;
                    texture.texture = t;

                    // TODO: check for id collisions
                    auto& img = loaded_images.emplace_back();
                    img.id = e.id;
                    img.texture_id = texture_id;
                    img.world_size = e.world_size;
                }
                else if constexpr(std::is_same_v<T, DrawImg>)
                {
                    images_to_be_drawn.push_back(e);
                }
                else if constexpr(std::is_same_v<T, CreateCamera>)
                {
                    camera = e;
                }
                else if constexpr(std::is_same_v<T, MoveCamera>)
                {
                    camera.world_coordinate = e.world_coordinate;
                }
                else if constexpr(std::is_same_v<T, Present>)
                {
                    SDL_RenderClear(renderer);

                    if (camera.id)
                        draw_images();

                    SDL_RenderPresent(renderer);
                    output_sink->put_event(RenderDone{});
                    images_to_be_drawn.clear();
                }
            },
            input_queue.front());

        input_queue.pop();
    }
}

void Graphics::draw_images()
{
    buffer_ss_images.clear();

    const auto camera_top_left = Point{
        .x = camera.world_coordinate.x - (int32_t)camera.world_size.width/2,
        .y = camera.world_coordinate.y + (int32_t)camera.world_size.height/2,
    };
    const auto camera_bottom_right = Point{
        .x = camera_top_left.x + (int32_t)camera.world_size.width,
        .y = camera_top_left.y - (int32_t)camera.world_size.height,
    };

    for (auto &img : images_to_be_drawn)
    {
        auto it_loaded = std::find_if(
            std::begin(loaded_images),
            std::end(loaded_images),
            [&](auto &loaded_img)
            { return loaded_img.id == img.id; });
        if (it_loaded == std::end(loaded_images))
        {
            // should not happen unless there is a bug somewhere (likely outside this class)
            // TODO: error handling
            continue;
        }

        // skip if image outside camera (TODO: fix for rotated images)
        auto img_left_top = Point{
            .x = img.world_coordinate.x - it_loaded->world_size.width/2,
            .y = img.world_coordinate.y + it_loaded->world_size.height/2,
        };
        auto img_right_bottom = Point{
            .x = img_left_top.x + it_loaded->world_size.width,
            .y = img_left_top.y - it_loaded->world_size.height,
        };

        if (img_right_bottom.x < camera_top_left.x ||
            img_right_bottom.y > camera_top_left.y ||
            img_left_top.x > camera_bottom_right.x ||
            img_left_top.y < camera_bottom_right.y)
        {
            continue;
        }

        // convert to screen space using camera and window resolution
        auto& ss_img = buffer_ss_images.emplace_back();
        ss_img.texture_id = it_loaded->texture_id;
        ss_img.coordinates = {
            .x = (img_left_top.x - camera_top_left.x) * (int)(window_resolution.width) / (int)camera.world_size.width,
            .y = (camera_top_left.y - img_left_top.y) * (int)(window_resolution.height) / (int)camera.world_size.height,
            .w = (int)(it_loaded->world_size.width * window_resolution.width / camera.world_size.width),
            .h = (int)(it_loaded->world_size.height * window_resolution.height / camera.world_size.height),
        };
        ss_img.layer = img.layer;
        ss_img.angle = (float)img.rotation / 100.f;
    }

    std::sort(
        std::begin(buffer_ss_images),
        std::end(buffer_ss_images),
        [](auto& img1, auto& img2){ return img1.layer < img2.layer; });

    // draw images
    for (auto& img : buffer_ss_images)
    {
        auto it_texture = std::find_if(
            std::begin(loaded_textures),
            std::end(loaded_textures),
            [&](auto& t) { return t.id == img.texture_id; });
        if (it_texture == std::end(loaded_textures))
        {
            // This should not happen unless there is a bug somewhere in this class
            // TODO: error handling
            continue;
        }

        SDL_RenderCopyEx(
            renderer, 
            it_texture->texture, 
            nullptr,
            &img.coordinates,
            img.angle,
            0,
            SDL_RendererFlip::SDL_FLIP_NONE);
    }
}

} // namespace tt
