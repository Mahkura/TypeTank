#include "CommParser.hpp"
#include <string_view>
#include <algorithm>
#include <optional>
#include <rapidjson/reader.h>

namespace tt
{
struct JsonFilter
{
    std::string filter;

    void push_key(std::string_view v)
    {
        filter.reserve(filter.size() + v.size() + 1);

        for (auto c : v)
        {
            if (c == '~')
                filter += "~0";
            else if (c == '/')
                filter += "~1";
            else if (c == '[')
                filter += "~2";
            else
                filter += c;
        }
    }

    void push_object()
    {
        if (filter.size() == 0 || filter.back() != '[')
            filter += '/';
    }

    void pop_object()
    {
        if (filter.back() == '[') // object in array removed, does not affect filter
            return;

        auto it_arr = std::find(filter.rbegin() + 1, filter.rend(), '[');
        const auto dist_arr = std::distance(filter.rbegin(), it_arr);
        auto it_obj = std::find(filter.rbegin() + 1, filter.rend(), '/');
        const auto dist_obj = std::distance(filter.rbegin(), it_obj) + 1; // remove object marker

        auto index = dist_arr;
        if (index > dist_obj)
            index = dist_obj;
        
        index = filter.size() - index; // reverse iterators, calculate index
        filter.erase(index);
    }

    void push_array()
    {
        filter += '[';
    }

    void pop_array()
    {
        auto it_arr = std::find(filter.rbegin() + 1, filter.rend(), '[');
        const auto dist_arr = std::distance(filter.rbegin(), it_arr);
        auto it_obj = std::find(filter.rbegin() + 1, filter.rend(), '/');
        const auto dist_obj = std::distance(filter.rbegin(), it_obj);

        auto index = dist_arr;
        if (index > dist_obj)
            index = dist_obj;
        
        index = filter.size() - index; // reverse iterators, calculate index
        filter.erase(index);
    }

    void pop_value()
    {
        if (filter.front() == '[') // arrays may have unnamed values
            return;

        auto it_arr = std::find(filter.rbegin(), filter.rend(), '[');
        const auto dist_arr = std::distance(filter.rbegin(), it_arr);
        auto it_obj = std::find(filter.rbegin(), filter.rend(), '/');
        const auto dist_obj = std::distance(filter.rbegin(), it_obj);

        auto index = dist_arr;
        if (index > dist_obj)
            index = dist_obj;
        
        index = filter.size() - index; // reverse iterators, calculate index
        filter.erase(index);
    }

    auto operator==(std::string_view o) -> bool
    {
        return filter == o;
    }
};

struct JsonParser
{
    using Ch = char;
    using SizeType = size_t;

    JsonFilter filter;
    EventSink* sink;

    // TODO: probably a better way to do this
    // TankMoved
    std::optional<std::string> tank_name;
    std::optional<uint16_t> x;
    std::optional<uint16_t> y;

    // TankCreated
    std::optional<std::string> created_tank_name;
    std::optional<uint16_t> created_x;
    std::optional<uint16_t> created_y;
    std::optional<uint8_t> created_width;
    std::optional<uint8_t> created_height;
    bool created_owned = false;

    void try_put_tank_moved()
    {
        if (tank_name.has_value() && x.has_value() && y.has_value())
        {
            sink->put_event(TankMoved{
                .name = tank_name.value(),
                .pos_x = x.value(),
                .pos_y = y.value()
            });
        }
    }

    void reset_tank_moved()
    {
        tank_name.reset();
        x.reset();
        y.reset();
    }

    void try_put_tank_created()
    {
        if (created_tank_name.has_value() &&
            created_x.has_value() &&
            created_y.has_value() &&
            created_width.has_value() &&
            created_height.has_value())
        {
            sink->put_event(TankCreated{
                .name = created_tank_name.value(),
                .flags = created_owned * TankCreated::Flags::MyTank,
                .pos_x = created_x.value(),
                .pos_y = created_y.value(),
                .width = created_width.value(),
                .height = created_height.value(),
            });
        }
    }

    void reset_tank_created()
    {
        created_tank_name.reset();
        created_x.reset();
        created_y.reset();
        created_width.reset();
        created_height.reset();
        created_owned = false;
    }

    bool Null()
    {
        filter.pop_value();
        return true;
    }

    bool Bool(bool b)
    {
        if (filter == "/tank_created[owned")
            created_owned = b;

        filter.pop_value();
        return true;
    }

    bool Int(int i)
    {
        filter.pop_value();
        return true;
    }
    bool Uint(unsigned i)
    {
        if (filter == "/tank_moved[x")
            x = i;
        else if (filter == "/tank_moved[y")
            y = i;
        else if (filter == "/tank_created[x")
            created_x = i;
        else if (filter == "/tank_created[y")
            created_y = i;
        else if (filter == "/tank_created[width")
            created_width = i;
        else if (filter == "/tank_created[height")
            created_height = i;

        filter.pop_value();
        return true;
    }
    bool Int64(int64_t i)
    {
        filter.pop_value();
        return true;
    }
    bool Uint64(uint64_t i)
    {
        filter.pop_value();
        return true;
    }
    bool Double(double d)
    {
        filter.pop_value();
        return true;
    }
    bool RawNumber(const Ch* str, SizeType length, bool copy) { return false; }

    bool String(const Ch* str, SizeType length, bool copy)
    {
        if (filter == "/tank_moved[name")
            tank_name = {str, length};
        else if (filter == "/tank_created[name")
            created_tank_name = {str, length};

        filter.pop_value();
        return true;
    }
    
    bool StartObject()
    {
        filter.push_object();
        return true;
    }

    bool EndObject(SizeType memberCount)
    {
        if (filter == "/tank_moved[")
        {
            try_put_tank_moved();
            reset_tank_moved();
        }
        if (filter == "/tank_created[")
        {
            try_put_tank_created();
            reset_tank_created();
        }

        filter.pop_object();
        return true;
    }
    bool StartArray()
    {
        filter.push_array();
        return true;
    }
    bool EndArray(SizeType elementCount)
    {
        filter.pop_array();
        return true;
    }
    bool Key(const Ch* str, SizeType length, bool copy)
    {
        filter.push_key(std::string_view(str, length));
        return true;
    }
};

struct StringViewStream
{
    using Ch = char;
    size_t index = 0;
    std::string_view buffer;

    Ch Peek() const
    {
        if (index >= buffer.size()) 
            return '\0';

        return buffer[index];
    }

    Ch Take()
    {
        if (index >= buffer.size()) 
            return '\0';

        return buffer[index++];
    }

    auto Tell() const -> size_t
    {
        return index;
    }

    Ch* PutBegin() { assert(false); return nullptr; }
    auto PutEnd(Ch*) -> size_t { assert(false); return 0; }
    void Put(Ch) { assert(false); }
    void Flush() { assert(false); }
};

void CommParser::register_input(EventSink& sink)
{
    sink.register_queue<DataReceived>(&input_queue);
}

void CommParser::execute()
{
    while (!input_queue.empty())
    {
        std::visit(
            [&](auto &&e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr(std::is_same_v<T, DataReceived>)
                {
                    if (expected_buffer_size == 0)
                    {
                        expected_buffer_size = (e.buffer[3] << 24) 
                            + (e.buffer[2] << 16)
                            + (e.buffer[1] << 8)
                            + e.buffer[0];

                        buffer.reserve(expected_buffer_size);
                        buffer.append(e.buffer + 4, &e.buffer[e.len]);
                    }
                    else
                    {
                        buffer.append(e.buffer, &e.buffer[e.len]);
                        // TODO: handle bytes after expected_buffer_size
                    }

                    if (buffer.size() == expected_buffer_size)
                    {
                        auto stream = StringViewStream{
                            .index = 0,
                            .buffer = buffer,
                        };
                        auto handler = JsonParser{
                            .sink = output_sink
                        };
                        auto reader = rapidjson::Reader{};
                        reader.Parse<rapidjson::kParseCommentsFlag>(stream, handler);
                        expected_buffer_size = 0; // TODO  test for this
                    }
                }
            },
            input_queue.front());

        input_queue.pop();
    }
}

} // namespace tt
