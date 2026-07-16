#include "settings.h"
#include "exceptions.h"
// #include <iostream>  // Если нужно будет вызывать imbue() с потоков.
#include <filesystem>
#include <locale>

namespace
{
    std::locale global_locale;
    std::string db_full_filename;
    const char *time_zone = "Etc/UTC";

}  // namespace

void Settings::setLocate (const char *locale)
{
    try
    {
        global_locale = std::locale{locale};
    }
    catch (const std::runtime_error &)
    {
        try
        {
            global_locale = std::locale{""};
        }
        catch (const std::runtime_error &exc)
        {
            throw SetLocaleError{exc.what()};
        }
    }

    std::locale::global(global_locale);

    // std::cout.imbue(global_locale);
    // std::cerr.imbue(global_locale);
    // std::cin.imbue(global_locale);
    // std::clog.imbue(global_locale);
}

void Settings::setDbFileName (const char *file_name)
{
    std::error_code er_code;
    std::filesystem::path current_path = std::filesystem::current_path(er_code);

    if (er_code)
    {
        throw GetCurrentPathError{er_code.value()};
    }

    current_path /= file_name;
    db_full_filename = std::move(current_path);
}

void Settings::setTimeZone (const char *zone)
{
    time_zone = zone;
}

// Возвращается копия, чтобы исходное значение не могло быть изменено.
std::string Settings::getDbFullFileName ()
{
    return db_full_filename;
}

const char *Settings::getTimeZone ()
{
    return time_zone;
}
