#pragma once
#include <events/EventQueue.hpp>
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>

namespace tt
{

struct EventSink
{
private:
    std::unordered_map<size_t, std::unordered_set<EventQueue*>> queues_for_types;

    template<typename T>
    void internal_register_queue(EventQueue* queue)
    {
        queues_for_types[typeid(T).hash_code()].emplace(queue);
    }

public:
    template<typename... T>
    void register_queue(EventQueue* queue)
    {
        (internal_register_queue<T>(queue), ...);
    }

    void put_event(Event event);
};

} // namespace tt
