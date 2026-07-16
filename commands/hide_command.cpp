#include "hide_command.h"
#include "../exceptions.h"
#include "../db/repository.h"
#include <iostream>

HideCommand::HideCommand (long long int task_id) : task_id{task_id} {}

void HideCommand::execute (const SQLiteRepository &repository)
{
    if (this->task_id <= 0)
    {
        throw CommandError{"Ошибка при скрытии задачи: id задачи меньше либо равен 0"};
    }

    TaskRow task = repository.getTask(this->task_id);

    if (task.is_hidden)
    {
        throw CommandError{"Ошибка при скрытии задачи: задача уже скрыта"};
    }

    task.is_hidden = true;
    repository.updateTask(task);

    std::cout << "Задача " << task_id << " скрыта\n";
}
