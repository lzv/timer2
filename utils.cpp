#include "utils.h"
#include <chrono>
#include <format>

void loadTZDB ()
{
    std::chrono::get_tzdb();
}

long long int getCurrentTimestamp ()
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

std::string getTimeLength (long long int seconds)
{
    const std::chrono::hh_mm_ss length{std::chrono::seconds{seconds}};

    if (length.hours().count() > 0)
    {
        return std::format(
            "{} ч. {} м. {} с", length.hours().count(), length.minutes().count(), length.seconds().count()
        );
    }

    if (length.minutes().count() > 0)
    {
        return std::format("{} м. {} с", length.minutes().count(), length.seconds().count());
    }

    return std::format("{} с", length.seconds().count());
}
