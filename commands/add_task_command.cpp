#include "add_task_command.h"
#include "../exceptions.h"
#include "../types/task_row.h"
#include "../db/repository.h"

//------------------------- Локальный код ----------------------------------

namespace
{
    // Удаление пробелов у строки в начале и в конце.

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

}  // namespace

//------------------------- end Локальный код -------------------------------

AddTaskCommand::AddTaskCommand (long long int parent_id, std::string name) : parent_id{parent_id}, name{std::move(name)}
{
    if (this->parent_id < 0)
    {
        throw CommandError{"Ошибка: id родительской задачи меньше 0"};
    }
    if (this->name.empty())
    {
        throw CommandError{"Ошибка: не указано имя задачи"};
    }

    trim(this->name);

    if (this->name.empty())
    {
        throw CommandError{"Ошибка: имя задачи не может состоять из пробелов"};
    }
}

void AddTaskCommand::execute (const SQLiteRepository &repository)
{
    TaskRow task;

    task.parent_id = this->parent_id;
    task.name = this->name;

    auto id = repository.addTask(task);

    std::cout << "Добавлена задача " << id << " (" << task.name << ")\n" << std::flush;
}
