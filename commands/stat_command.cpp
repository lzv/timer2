#include "stat_command.h"
#include <iostream>
#include <format>
#include <string>
#include "../utils.h"
#include "../utils_inline.h"
#include "../exceptions.h"
#include "../db/repository.h"

namespace
{
    TimeEntity getGlobalTimeEntity (StatCommand::TimeEntity te)
    {
        if (te == StatCommand::TimeEntity::MONTH)
        {
            return TimeEntity::MONTH;
        }
        if (te == StatCommand::TimeEntity::WEEK)
        {
            return TimeEntity::WEEK;
        }
        return TimeEntity::DAY;
    }

    long long int print_tasks (const TasksVec &tasks, const std::string &prefix)
    {
        long long int sum = 0;

        for (const auto &task : tasks)
        {
            auto task_length = task.getFullLength();
            sum += task_length;

            std::cout << std::format(
                "{}[{}{}{}] ({}) {}\n",
                prefix,
                task.data.id,
                task.data.is_hidden ? ",H" : "",
                task.is_active() ? ",*" : "",
                getTimeLength(task_length),
                task.data.name
            );

            if (!task.sub_tasks.empty())
            {
                print_tasks(task.sub_tasks, prefix + "    ");
            }
        }

        return sum;
    }

}  // namespace

StatCommand::StatCommand (TimeEntity time_entity, int shift) : time_entity{time_entity}, shift{shift} {}

void StatCommand::execute (const SQLiteRepository &repository)
{
    auto result = getPeriodBoundaries(getGlobalTimeEntity(this->time_entity), this->shift);

    if (!result)
    {
        throw CommandError{"Ошибка при получении границ временного диапазона"};
    }

    // Выведем границы для проверки.
    std::cout << std::format(
        "Вывод статистики с {:L%Oe %B %Y г. %H:%M:%S} по {:L%Oe %B %Y г. %H:%M:%S} включительно:\n\n",
        getZonedTime(result.value().first),
        getZonedTime(result.value().second - 1)
    );

    // Запрашиваем список всех задач и обрезаем по указанному периоду.
    auto tasks = repository.getFullTasks(0, TaskVisibility::ALL);
    tasks.crop(result.value().first, result.value().second);

    if (tasks.empty())
    {
        std::cout << "Для указанного периода задачи не найдены\n";
    }
    else
    {
        auto tasks_length = print_tasks(tasks, "");

        std::cout << std::format("\nСуммарное время: {}\n", getTimeLength(tasks_length));
    }
}
