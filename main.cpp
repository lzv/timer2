#include "exceptions.h"
#include "settings.h"
#include "main_loop.h"
#include "utils.h"

int main ([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    try
    {
        loadTZDB();  // Загрузка БД временных зон до изменения локали.

        Settings::setTimeZone("Asia/Krasnoyarsk");
        Settings::setLocate("ru_RU.utf8");
        Settings::setDbFileName("timer.db");

        MainLoop::start();
    }
    catch (const ExceptionWithMessage &exp)
    {
        exp.printMessage();
        return 1;
    }

    return 0;
}
