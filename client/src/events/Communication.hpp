#pragma once
#include <cstdint>
#include <string>

namespace tt
{
struct Connect
{
    std::string url;
    uint16_t port;
};

struct Connected
{
    std::string url;
};

struct DataReceived
{
    uint8_t buffer[255];
    uint8_t len;
};

struct SendData : DataReceived {};

struct Disconnected
{
    std::string url;
    // TODO reason
};
} // namespace tt
