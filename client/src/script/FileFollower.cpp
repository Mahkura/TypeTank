#include <script/FileFollower.hpp>
#include <filesystem>
#include <fstream>

namespace tt
{
namespace fs = std::filesystem;

auto FileFollower::create_files() -> bool
{
    auto cur = fs::current_path();
    auto script_file_path = cur / "typetanks.script";
    auto script_file = std::fstream(
        script_file_path,
        std::ios_base::trunc | std::ios_base::out);
    script_file.write("", 0);
    if (!script_file.good())
        return false;
    
    file_to_follow = script_file_path.string();
    file_last_hash = std::hash<std::string>{}("");
    return true;
}

void FileFollower::execute()
{
    auto ec = std::error_code{};
    auto write_time = fs::last_write_time(file_to_follow, /*out*/ec);
    if (ec)
        return;
    
    auto last_modified = (uint64_t)write_time.time_since_epoch().count();
    if (file_last_modified == 0)
    {
        file_last_modified = last_modified;
        return;
    }

    if (file_last_modified != last_modified)
    {
        file_last_modified = last_modified;

        auto file = std::fstream(
            file_to_follow,
            std::ios_base::in | std::ios_base::ate | std::ios_base::binary);
        if (file)
        {
            const auto file_size = file.tellg();
            auto file_content = std::string(file_size, 0);
            file.seekg(std::ios::beg);
            file.read(file_content.data(), file_size);

            auto hash = std::hash<std::string>{}(file_content);
            if (file_last_hash != hash)
            {
                file_last_hash = hash;                
                output_sink->put_event(ScriptChanged{
                    .script_content = std::make_shared<std::string>(
                        std::move(file_content)) // OPTIMIZE: reuse string buffer
                });
            }
        }
    }
}

} // namespace tt
