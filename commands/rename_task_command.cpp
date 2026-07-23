#include "rename_task_command.h"
#include <utility>
#include "../exceptions.h"
#include "../types/task_row.h"
#include "../db/repository.h"
#include "../utils.h"

RenameTaskCommand::RenameTaskCommand (long long int id, std::string new_name) : id{id}, new_name{std::move(new_name)}
{
    if (this->id <= 0)
    {
        throw CommandError{"Ошибка: id задачи меньше либо равен 0"};
    }
    if (this->new_name.empty())
    {
        throw CommandError{"Ошибка: не указано новое имя задачи"};
    }

    trim(this->new_name);

    if (this->new_name.empty())
    {
        throw CommandError{"Ошибка: новое имя задачи не может состоять из пробелов"};
    }
}

void RenameTaskCommand::execute (const SQLiteRepository &repository)
{
    TaskRow task = repository.getTask(this->id);
    task.name = this->new_name;
    repository.updateTask(task);

    std::cout << "Название задачи " << this->id << " изменено\n";
}
