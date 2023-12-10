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
            .size_dm = { .width = 20, .height = 50 },
        });
        test.sink.put_event(tt::TankCreated{
            .name = "tanky",
            .flags = tt::TankCreated::MyTank,
            .pos_dm = { .x = 100, .y = 110 },
            .size_dm = { .width = 20, .height = 50 },
        });
        test.sink.put_event(tt::TankCreated{
            .name = "tanky2",
            .flags = tt::TankCreated::MyTank,
            .pos_dm = { .x = 200, .y = 210 },
            .size_dm = { .width = 20, .height = 50 },
        });

        test.execute();

        expect((test.output_queue.size() == 1) >> fatal);
        auto e = std::get<tt::CreateCamera>(test.output_queue.front());
        expect(e.id == 1);
        expect(e.world_coordinate.x == 10'000);
        expect(e.world_coordinate.y == 11'000);
        expect(e.world_size.width == 40'000);
        expect(e.world_size.height == 40'000);
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

        expect((test.output_queue.size() == 1) >> fatal);
        auto e = std::get<tt::MoveCamera>(test.output_queue.front());
        expect(e.id == 1);
        expect(e.world_coordinate.x == 10'500);
        expect(e.world_coordinate.y == 11'500);
    };
    // TODO: Map event after TankCreated, etc.
    // TODO: FrameStarted before Map, TankCreated, etc.
    return 0;
}