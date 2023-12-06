#include <net/CommParser.hpp>

#define BOOST_UT_DISABLE_MODULE
#include <ut.hpp>

using namespace ::boost::ut;

struct Fixture
{
    tt::EventQueue output_queue;
    tt::EventSink sink;
    tt::CommParser sut;

    Fixture()
    {
        sut.register_input(sink);
        sut.output_sink = &sink;
        sink.register_queue<tt::TankCreated, tt::TankMoved>(
            &output_queue);
    }

    void execute()
    {
        sut.execute();
    }

    void send_json(std::string_view json)
    {
        if (json.empty())
            throw std::runtime_error("empty test JSON");

        // split into as many DataReceived as necessary
        // put total message size as first 4 bytes
        const auto json_size = json.size();
        auto data_received = tt::DataReceived{};
        data_received.buffer[0] = json_size & 0xFF;
        data_received.buffer[1] = json_size >> 8 & 0xFF;
        data_received.buffer[2] = json_size >> 16 & 0xFF;
        data_received.buffer[3] = json_size >> 24 & 0xFF;
        const auto count = (sizeof(data_received.buffer) - 4) < json.size() ?
            (sizeof(data_received.buffer) - 4)
            : json.size();
        memcpy(data_received.buffer + 4, json.data(), count);
        data_received.len = count + 4;
        json = json.substr(count);
        sink.put_event(data_received);

        while (!json.empty())
        {
            auto data_received = tt::DataReceived{};
            const auto count = sizeof(data_received.buffer) < json.size() ?
                sizeof(data_received.buffer)
                : json.size();
            memcpy(data_received.buffer, json.data(), count);
            data_received.len = count;
            json = json.substr(count);
            sink.put_event(data_received);
        }
    }
};

int main(int argc, char** argv)
{
    "when communication contains tankmove"_test = []
    {
        auto test = Fixture{};
        test.send_json(R"({
            "tank_moved": [
                {
                    "name": "some Tank name",
                    "x": 12442, // left upper corned of Tank
                    "y": 23
                },
                {
                    "name": "another Tank name",
                    "x": -442, // x and y are absolute positions, negative numbers are not supported,
                    "y": 0
                },
                {
                    "name": "yet another Tank name",
                    "x": 0,
                    "y": 11
                }
            ]
        })");

        test.execute();
        expect((test.output_queue.size() == 2) >> fatal);
        auto d = std::get<tt::TankMoved>(test.output_queue.front());
        expect(d.name == "some Tank name");
        expect(d.pos_x == 12442u);
        expect(d.pos_y == 23u);
        test.output_queue.pop();
        d = std::get<tt::TankMoved>(test.output_queue.front());
        expect(d.name == "yet another Tank name");
        expect(d.pos_x == 0u);
        expect(d.pos_y == 11u);
    };
    "when communication contains tankcreated"_test = []
    {
        auto test = Fixture{};
        test.send_json(R"({
            "tank_moved": [
                {
                    "name": "some Tank name",
                    "x": 12442, // left upper corned of Tank
                    "y": 23
                }
            ],
            "tank_created": [
                {
                    "name": "new Tank",
                    "owned": true, // indicates that this is our character

                    // position and size data restricted to 16 bits
                    // unit is "1/10th of meter", we map them to pixels elsewhere
                    // this limits the maximum play area to 6553.5 meters
                    // internally the server should use more accurate position / velocity data
                    "x": 500,
                    "y": 250,
                    "width": 25, // 2.5m wide tank
                    "height": 50 
                }
            ]
        })");

        test.execute();
        expect((test.output_queue.size() == 2) >> fatal);
        auto d = std::get<tt::TankMoved>(test.output_queue.front());
        expect(d.name == "some Tank name");
        expect(d.pos_x == 12442u);
        expect(d.pos_y == 23u);
        test.output_queue.pop();
        auto c = std::get<tt::TankCreated>(test.output_queue.front());
        expect(c.name == "new Tank");
        expect(c.flags == tt::TankCreated::MyTank);
        expect(c.pos_x == 500u);
        expect(c.pos_y == 250u);
        expect(c.width == 25u);
        expect(c.height == 50u);
    };
    // TODO: test when DataReceived contain both the end of one JSON and the beginning of another JSON

    return 0;
}
