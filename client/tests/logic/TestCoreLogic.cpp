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
        sut.register_queue(/*inout*/sink);
        sink.register_queue<
            tt::SetPixelRatio,
            tt::DrawImg,
            tt::RequiresImg,
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
    "blaa"_test = []
    {
        auto test = Fixture{};
        test.sink.put_event(tt::Map{
            .width_m = 300,
            .height_m = 300,
        });

        test.execute();

        expect(test.output_queue.size() == 1);
    };
    // TODO: Map event after TankCreated, etc.
    return 0;
}