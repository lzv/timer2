#include "command_factory.h"
#include "../exceptions.h"
#include "exit_command.h"
#include "add_task_command.h"
#include "help_command.h"
#include "list_tasks_command.h"
#include "start_command.h"
#include "stop_command.h"
#include "hide_command.h"
#include "show_command.h"

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

    inline void check_id (const std::string &id, const char *err_message)
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
        "Команда \"список задач\" выведет иерархический список видимых задач. "
            "Если добавить \"всех\", будут выведены также скрытые задачи.\n"
        "Команда \"старт ID\" запустит задачу с указанным ID.\n"
        "Команда \"стоп ID\" остановит задачу с указанным ID. Если не указать ID, будут остановлены все задачи.\n"
        "Команда \"скрыть ID\" скроет задачу из списка всех задач (статистика не будет затронута).\n"
        "Команда \"показать ID\" отменит скрытие указанной задачи.";
    // clang-format on

}  // namespace

//------------------------- end Локальный код -------------------------------

// Фабричная функция инкапсулирует обработку входных текстовых команд и связывает их с классами.
// Конкретные классы команд знают только свои функции.

std::unique_ptr<CommandBase> getCommand (const std::vector<std::string> &words)
{
    check_vector_size(words, 1, "Ошибка: команда не указана");

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
            check_id(words[2], "Ошибка: некорректный id родительской задачи");

            return std::make_unique<AddTaskCommand>(std::stoll(words[2]), merge_words(words, 3));
        }
    }

    if (words[0] == "список")
    {
        check_vector_size(words, 2, "Ошибка: недостаточно параметров команды");

        if (words[1] == "задач")
        {
            if (words.size() > 2)
            {
                if (words[2] != "всех")
                {
                    throw CommandError{"Ошибка: некорректный параметр команды"};
                }

                return std::make_unique<ListTasksCommand>(true);
            }

            return std::make_unique<ListTasksCommand>(false);
        }
    }

    if (words[0] == "старт")
    {
        check_vector_size(words, 2, "Ошибка: недостаточно параметров команды");
        check_id(words[1], "Ошибка: некорректный id задачи");

        return std::make_unique<StartCommand>(std::stoll(words[1]));
    }

    if (words[0] == "стоп")
    {
        if (words.size() > 1)
        {
            check_id(words[1], "Ошибка: некорректный id задачи");

            return std::make_unique<StopCommand>(std::stoll(words[1]));
        }

        return std::make_unique<StopCommand>();
    }

    if (words[0] == "скрыть")
    {
        check_vector_size(words, 2, "Ошибка: недостаточно параметров команды");
        check_id(words[1], "Ошибка: некорректный id задачи");

        return std::make_unique<HideCommand>(std::stoll(words[1]));
    }

    if (words[0] == "показать")
    {
        check_vector_size(words, 2, "Ошибка: недостаточно параметров команды");
        check_id(words[1], "Ошибка: некорректный id задачи");

        return std::make_unique<ShowCommand>(std::stoll(words[1]));
    }

    if (words[0] == "помощь")
    {
        return std::make_unique<HelpCommand>(help_string);
    }

    throw CommandError{"Ошибка: неизвестная команда"};
}
