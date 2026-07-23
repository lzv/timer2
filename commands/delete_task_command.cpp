#include "delete_task_command.h"
#include "../exceptions.h"
#include "../db/repository.h"
#include <iostream>

DeleteTaskCommand::DeleteTaskCommand (long long int id) : id{id}
{
    if (this->id <= 0)
    {
        throw CommandError{"Ошибка: id задачи меньше либо равен 0"};
    }
}

void DeleteTaskCommand::execute (const SQLiteRepository &repository)
{
    (void) repository.getTask(this->id);  // Проверка на существование задачи.
    repository.deleteTask(this->id);

    std::cout << "Удаление задачи " << this->id << " и всех ее подзадач выполнено\n";
}
