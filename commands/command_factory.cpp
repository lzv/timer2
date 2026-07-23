#include "command_factory.h"
#include "../exceptions.h"
#include "list_active_command.h"
#include "exit_command.h"
#include "add_task_command.h"
#include "help_command.h"
#include "list_tasks_command.h"
#include "start_command.h"
#include "stop_command.h"
#include "hide_command.h"
#include "show_command.h"
#include "stat_command.h"

//------------------------- Локальный код ----------------------------------

namespace
{
    inline void check_vector_size (
        const std::vector<std::string> &vec, std::vector<std::string>::size_type size, const char *err_message
    )
    {
        if (vec.size() < size)
        {
            throw CommandError{err_message};
        }
    }

    inline void check_numbers_only (const std::string &id, const char *err_message)
    {
        if (id.empty() || id.find_first_not_of("0123456789") != std::string::npos)
        {
            throw CommandError{err_message};
        }
    }

    inline std::string merge_words (const std::vector<std::string> &words, std::vector<std::string>::size_type start)
    {
        std::string result = words[start];

        for (std::vector<std::string>::size_type i = start + 1; i < words.size(); ++i)
        {
            result += " " + words[i];
        }

        return result;
    }

    // clang-format off
    constexpr const char *help_string =
        "Доступны команды: добавить, помощь, выход.\n"
        "Для добавления задачи наберите \"добавить задачу PID Название задачи\", где PID - ID родительской задачи. "
            "Если добавляемая задача должна быть высшей в иерархии, укажите 0.\n"
        "Команда \"задачи\" выведет иерархический список видимых задач. "
            "Если добавить \"все\", будут выведены также скрытые задачи.\n"
        "Команда \"старт ID\" запустит задачу с указанным ID.\n"
        "Команда \"стоп ID\" остановит задачу с указанным ID. Если не указать ID, будут остановлены все задачи.\n"
        "Команда \"скрыть ID\" скроет задачу из списка всех задач (статистика не будет затронута).\n"
        "Команда \"показать ID\" отменит скрытие указанной задачи.\n"
        "Команда \"стат T S\" покажет статистику по всем делам за время T со сдвигом в прошлое S. "
            "T может быть \"день\", \"неделя\" или \"месяц\", статистика показывается за весь указанный период. "
            "S - число от 0. При 0 будет показан текущий день/неделя/месяц, при 1 - предыдущий период, и т.д. "
            "Значение по умолчанию для T это \"день\", для S это 0. Если указан S, нужно обязательно указать T.\n"
        "Просто нажатие enter (пустая команда) выведет список активных задач.";
    // clang-format on

}  // namespace

//------------------------- end Локальный код -------------------------------

// Фабричная функция инкапсулирует обработку входных текстовых команд и связывает их с классами.
// Конкретные классы команд знают только свои функции.

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
std::unique_ptr<CommandBase> getCommand (const std::vector<std::string> &words)
{
    if (words.empty())
    {
        return std::make_unique<ListActiveCommand>();
    }

    if (words[0] == "выход")
    {
        return std::make_unique<ExitCommand>();
    }

    if (words[0] == "добавить")
    {
        check_vector_size(words, 2, "Ошибка: недостаточно параметров команды");

        if (words[1] == "задачу")
        {
            check_vector_size(words, 4, "Ошибка: недостаточно параметров команды");
            check_numbers_only(words[2], "Ошибка: некорректный id родительской задачи");

            return std::make_unique<AddTaskCommand>(std::stoll(words[2]), merge_words(words, 3));
        }
    }

    if (words[0] == "задачи")
    {
        if (words.size() > 1)
        {
            if (words[1] != "все")
            {
                throw CommandError{"Ошибка: некорректный параметр команды"};
            }

            return std::make_unique<ListTasksCommand>(true);
        }

        return std::make_unique<ListTasksCommand>(false);
    }

    if (words[0] == "старт")
    {
        check_vector_size(words, 2, "Ошибка: недостаточно параметров команды");
        check_numbers_only(words[1], "Ошибка: некорректный id задачи");

        return std::make_unique<StartCommand>(std::stoll(words[1]));
    }

    if (words[0] == "стоп")
    {
        if (words.size() > 1)
        {
            check_numbers_only(words[1], "Ошибка: некорректный id задачи");

            return std::make_unique<StopCommand>(std::stoll(words[1]));
        }

        return std::make_unique<StopCommand>();
    }

    if (words[0] == "скрыть")
    {
        check_vector_size(words, 2, "Ошибка: недостаточно параметров команды");
        check_numbers_only(words[1], "Ошибка: некорректный id задачи");

        return std::make_unique<HideCommand>(std::stoll(words[1]));
    }

    if (words[0] == "показать")
    {
        check_vector_size(words, 2, "Ошибка: недостаточно параметров команды");
        check_numbers_only(words[1], "Ошибка: некорректный id задачи");

        return std::make_unique<ShowCommand>(std::stoll(words[1]));
    }

    if (words[0] == "стат")
    {
        if (words.size() == 1)
        {
            return std::make_unique<StatCommand>();
        }

        auto get_time_entity = [] (const std::string &word) -> StatCommand::TimeEntity
        {
            if (word == "день")
            {
                return StatCommand::TimeEntity::DAY;
            }
            if (word == "неделя")
            {
                return StatCommand::TimeEntity::WEEK;
            }
            if (word == "месяц")
            {
                return StatCommand::TimeEntity::MONTH;
            }
            throw CommandError{"Ошибка: некорректный параметр команды"};
        };

        if (words.size() == 2)
        {
            return std::make_unique<StatCommand>(get_time_entity(words[1]));
        }

        if (words.size() == 3)
        {
            check_numbers_only(words[2], "Ошибка: некорректный параметр команды");

            return std::make_unique<StatCommand>(get_time_entity(words[1]), std::stoi(words[2]));
        }

        throw CommandError{"Ошибка: слишком много параметров команды"};
    }

    if (words[0] == "помощь")
    {
        return std::make_unique<HelpCommand>(help_string);
    }

    throw CommandError{"Ошибка: неизвестная команда"};
}
