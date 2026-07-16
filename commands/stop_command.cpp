#include "stop_command.h"
#include "../exceptions.h"
#include "../types/periods_vec.h"
#include "../types/period_row.h"
#include "../db/repository.h"
#include "../utils.h"
#include <iostream>

StopCommand::StopCommand () = default;

StopCommand::StopCommand (long long int task_id) : task_id{task_id} {}

void StopCommand::execute (const SQLiteRepository &repository)
{
    if (this->task_id < 0)
    {
        throw CommandError{"Ошибка при остановке задачи: id задачи меньше 0"};
    }

    // Останавливаем все активные задачи.
    if (this->task_id == 0)
    {
        if (repository.stopAllTasks() > 0)
        {
            std::cout << "Все автивные задачи остановлены\n";
        }
        else
        {
            std::cout << "Активные задачи не найдены\n";
        }

        return;
    }

    // Указан id задачи, нужно остановить только ее.

    PeriodsVec periods = repository.getPeriods(this->task_id);

    if (!periods.is_active())
    {
        throw CommandError{"Ошибка при остановке задачи: задача не найдена, либо у задачи нет активных периодов"};
    }

    auto cur_timestamp = getCurrentTimestamp();

    for (auto &period : periods)
    {
        if (period.is_active())
        {
            period.end = cur_timestamp;
            repository.updatePeriod(period);
        }
    }

    std::cout << "Задача " << task_id << " остановлена\n";
}
