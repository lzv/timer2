#include "show_command.h"
#include "../exceptions.h"
#include "../db/repository.h"
#include <iostream>

ShowCommand::ShowCommand (long long int task_id) : task_id{task_id} {}

void ShowCommand::execute (const SQLiteRepository &repository)
{
    if (this->task_id <= 0)
    {
        throw CommandError{"Ошибка при отмене скрытия задачи: id задачи меньше либо равен 0"};
    }

    TaskRow task = repository.getTask(this->task_id);

    if (!task.is_hidden)
    {
        throw CommandError{"Ошибка при отмене скрытия задачи: задача уже видима"};
    }

    task.is_hidden = false;
    repository.updateTask(task);

    std::cout << "Скрытие задачи " << task_id << " отменено\n";
}
