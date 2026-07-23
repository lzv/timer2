#include "utils.h"
#include <chrono>
#include <format>
#include "settings.h"

void loadTZDB ()
{
    std::chrono::get_tzdb();
}

long long int getCurrentSysTimestamp ()
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

std::optional<std::pair<long long int, long long int> > getPeriodBoundaries (TimeEntity entity, int shift)
{
    namespace ch = std::chrono;

    // Текущая временная метка unix.
    const ch::sys_time sys_now = ch::system_clock::now();

    // Указанная в настройках временная зона.
    const ch::time_zone *tz = ch::locate_zone(Settings::getTimeZone());

    // Локальное время в виде количества дней.
    ch::local_days local_days = ch::floor<ch::days>(tz->to_local(sys_now));

    // Преобразуем локальные дни в метки времени unix.
    auto get_result = [tz] (ch::local_days begin, ch::local_days end) -> std::pair<long long int, long long int>
    {
        const ch::sys_seconds sys_begin_sec = tz->to_sys(begin, ch::choose::earliest);
        const ch::sys_seconds sys_end_sec = tz->to_sys(end, ch::choose::earliest);

        return {sys_begin_sec.time_since_epoch().count(), sys_end_sec.time_since_epoch().count()};
    };

    if (entity == TimeEntity::DAY)
    {
        local_days -= ch::days{shift};

        return get_result(local_days, local_days + ch::days{1});
    }

    if (entity == TimeEntity::WEEK)
    {
        local_days -= ch::weekday{local_days} - ch::Monday;  // Понедельник текущей недели.
        local_days -= ch::days{shift * 7};                   // NOLINT magic number

        return get_result(local_days, local_days + ch::days{7});  // NOLINT magic number
    }

    if (entity == TimeEntity::MONTH)
    {
        const ch::year_month_day local_ymd{local_days};  // Локальная дата - год, месяц, день.
        ch::year_month_day local_month{local_ymd.year(), local_ymd.month(), ch::day{1}};
        local_month -= ch::months{shift};

        return get_result(ch::local_days{local_month}, ch::local_days{local_month + ch::months{1}});
    }

    return {};
}

void trim (std::string &s)
{
    const char *spaces = " \t\n\r";

    // Поиск пробелов в конце строки.
    auto pos = s.find_last_not_of(spaces);

    // Если true, строка состоит из пробелов либо пуста.
    if (pos == std::string::npos)
    {
        s.clear();
        return;
    }

    s.erase(pos + 1);

    // Т.к. уже проверено, что в стоке есть символы кроме пробелов, повторно проверять на npos не нужно.
    s.erase(0, s.find_first_not_of(spaces));
}
