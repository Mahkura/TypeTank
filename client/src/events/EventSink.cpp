#include <events/EventSink.hpp>

namespace tt
{

void EventSink::put_event(Event event)
{
    std::visit([&](auto&& e)
    {
        for (auto queue : queues_for_types[typeid(e).hash_code()])
            queue->push(e);
    },
    event);
}
} // namespace tt
