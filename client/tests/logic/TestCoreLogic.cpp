    #include <logic/CoreLogic.hpp>

#define BOOST_UT_DISABLE_MODULE
#include "ut.hpp"

using namespace ::boost::ut;

struct Fixture
{
    tt::EventSink sink;
    tt::EventQueue output_queue;
    tt::CoreLogic sut;

    Fixture()
    {
        sut.output_sink = &sink;
        sut.register_queue(/*inout*/sink);
        sink.register_queue<
            tt::DrawImg,
            tt::RequiresImg,
            tt::CreateCamera,
            tt::MoveCamera,
            tt::Present>(
                /*inout*/&output_queue);
    }

    void execute()
    {
        sink.put_event(tt::FrameStarted{
            .usec_since_last_frame = 16000,
        });
        sut.execute();
    }
};

int main(int, char**)
{
    "Camera is created at first owned tank position in millimeters"_test = []
    {
        auto test = Fixture{};
        test.sink.put_event(tt::Map{
            .width_m = 300,
            .height_m = 300,
        });
        test.sink.put_event(tt::TankCreated{
            .name = "meanie",
            .pos_dm = { .x = 20, .y = 20 },
            .size_dm = { .width = 21, .height = 50 },
        });
        test.sink.put_event(tt::TankCreated{
            .name = "tanky",
            .flags = tt::TankCreated::MyTank,
            .pos_dm = { .x = 100, .y = 110 },
            .size_dm = { .width = 22, .height = 50 },
        });
        test.sink.put_event(tt::TankCreated{
            .name = "tanky2",
            .flags = tt::TankCreated::MyTank,
            .pos_dm = { .x = 200, .y = 210 },
            .size_dm = { .width = 23, .height = 50 },
        });

        test.execute();

        expect((test.output_queue.size() == 7_u64) >> fatal);
        auto e = std::get<tt::CreateCamera>(test.output_queue.front());
        expect(e.id == 1);
        expect(e.world_coordinate.x == 10'000);
        expect(e.world_coordinate.y == 11'000);
        expect(e.world_size.width == 40'000);
        expect(e.world_size.height == 40'000);
        test.output_queue.pop();
        auto d = std::get<tt::RequiresImg>(test.output_queue.front());
        expect(d.img_path == "tank.svg");
        expect(d.id == 1_u32);
        expect(d.world_size.width = 2'100);
        expect(d.world_size.height = 5'000);
        test.output_queue.pop();
        d = std::get<tt::RequiresImg>(test.output_queue.front());
        expect(d.img_path == "tank.svg");
        expect(d.id == 2);
        expect(d.world_size.width = 2'200);
        expect(d.world_size.height = 5'000);
        test.output_queue.pop();
        d = std::get<tt::RequiresImg>(test.output_queue.front());
        expect(d.img_path == "tank.svg");
        expect(d.id == 3);
        expect(d.world_size.width = 2'300);
        expect(d.world_size.height = 5'000);
        test.output_queue.pop();
        auto i = std::get<tt::DrawImg>(test.output_queue.front());
        expect(i.id = 1);
        expect(i.layer == 10);
        expect(i.rotation == 0);
        expect(i.world_coordinate.x == 2'000_i32);
        expect(i.world_coordinate.y == 2'000_i32);
        test.output_queue.pop();
        i = std::get<tt::DrawImg>(test.output_queue.front());
        expect(i.id = 2);
        expect(i.layer == 10);
        expect(i.rotation == 0);
        expect(i.world_coordinate.x == 10'000_i32);
        expect(i.world_coordinate.y == 11'000_i32);
        test.output_queue.pop();
        i = std::get<tt::DrawImg>(test.output_queue.front());
        expect(i.id = 3);
        expect(i.layer == 10);
        expect(i.rotation == 0);
        expect(i.world_coordinate.x == 20'000_i32);
        expect(i.world_coordinate.y == 21'000_i32);
        test.output_queue.pop();
    };

    "Moving tank moves camera"_test = []
    {
        auto test = Fixture{};
        test.sink.put_event(tt::Map{
            .width_m = 300,
            .height_m = 300,
        });
        test.sink.put_event(tt::TankCreated{
            .name = "tanky",
            .flags = tt::TankCreated::MyTank,
            .pos_dm = { .x = 100, .y = 110 },
            .size_dm = { .width = 20, .height = 50 },
        });

        test.execute();
        while (!test.output_queue.empty())
            test.output_queue.pop();

        test.sink.put_event(tt::TankMoved{
            .name = "tanky",
            .pos_dm = { .x = 105, .y = 115 },
        });
        test.execute();

        expect((test.output_queue.size() == 2_u64) >> fatal);
        auto e = std::get<tt::MoveCamera>(test.output_queue.front());
        expect(e.id == 1);
        expect(e.world_coordinate.x == 10'500_i32);
        expect(e.world_coordinate.y == 11'500_i32);
        test.output_queue.pop();
        auto d = std::get<tt::DrawImg>(test.output_queue.front());
        expect(d.id == 1);
        expect(d.world_coordinate.x == 10'500_i32);
        expect(d.world_coordinate.y == 11'500_i32);
    };

    "No events results in no events"_test = []
    {
        auto test = Fixture{};
        test.execute();
        expect(test.output_queue.size() == 0_u64);
    };


    // TODO: Map event after TankCreated, etc.
    // TODO: FrameStarted before Map, TankCreated, etc.
    return 0;
}