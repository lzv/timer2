#include "start_command.h"
#include "../exceptions.h"
#include "../types/periods_vec.h"
#include "../db/repository.h"
#include <iostream>

StartCommand::StartCommand (long long int task_id) : task_id{task_id} {}

void StartCommand::execute (const SQLiteRepository &repository)
{
    if (this->task_id <= 0)
    {
        throw CommandError{"Ошибка при старте задачи: id задачи меньше либо равен 0"};
    }

    const PeriodsVec periods = repository.getPeriods(this->task_id);

    if (periods.is_active())
    {
        throw CommandError{"Ошибка при старте задачи: задача уже запущена"};
    }

    repository.addActivePeriodForTask(task_id);

    std::cout << "Задача " << task_id << " запущена\n";
}
