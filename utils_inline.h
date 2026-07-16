#pragma once

#include <chrono>
#include "settings.h"

inline auto getZonedTime (long long int timestamp)
{
    // clang-format off
    return std::chrono::zoned_time{
        Settings::getTimeZone(), 
        std::chrono::sys_seconds{std::chrono::seconds(timestamp)}
    };
    // clang-format on
}
