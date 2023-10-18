#include <events/EventSink.hpp>

#define BOOST_UT_DISABLE_MODULE
#include <ut.hpp>

using namespace ::boost::ut;

int main(int argc, char** argv)
{
    "Can push events into sink and have correct queue(s) receive it"_test = []
    {
        auto queue1 = tt::EventQueue{};
        auto queue2 = tt::EventQueue{};
        auto queue3 = tt::EventQueue{};
        auto sink = tt::EventSink{};
        sink.register_queue<tt::UnitId>(&queue1);
        sink.register_queue<tt::FrameStarted>(&queue2);
        sink.register_queue<tt::UnitId, tt::FrameStarted>(&queue3);
        sink.put_event(tt::UnitId{});
        sink.put_event(tt::FrameStarted{});

        expect(queue1.size() == 1);
        (void)std::get<tt::UnitId>(queue1.front());
        expect(queue2.size() == 1);
        (void)std::get<tt::FrameStarted>(queue2.front());
        expect(queue3.size() == 2);
        (void)std::get<tt::UnitId>(queue3.front());
        queue3.pop();
        (void)std::get<tt::FrameStarted>(queue3.front());
    };
    return 0;
}