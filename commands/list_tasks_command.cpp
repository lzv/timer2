#include "list_tasks_command.h"
#include <iostream>
#include <string>
#include <format>
#include "../types/task.h"
#include "../db/repository.h"
#include "../utils.h"
#include "../utils_inline.h"

ListTasksCommand::ListTasksCommand (bool show_all) : show_all{show_all} {}

namespace
{
    void print_tasks (const TasksVec &tasks, const std::string &prefix)
    {
        for (const auto &task : tasks)
        {
            auto task_full_length = task.getFullLength();
            std::string from_to;

            if (task_full_length > 0)
            {
                from_to = std::format(
                    "с {:%d.%m.%Y %T} по {:%d.%m.%Y %T}, ",
                    getZonedTime(task.getFirstTimePoint()),
                    getZonedTime(task.getLastTimePoint())
                );
            }

            std::cout << std::format(
                "{}[{}{}{}{}] {} ({}, {}добавлено {:%d.%m.%Y %T})\n",
                prefix,
                task.data.id,
                task.data.is_hidden ? ",H" : "",
                task.is_active() ? ",*" : "",
                task.is_active_self() ? "*" : "",
                task.data.name,
                getTimeLength(task_full_length),
                from_to,
                getZonedTime(task.data.created_timestamp)
            );

            if (!task.sub_tasks.empty())
            {
                print_tasks(task.sub_tasks, prefix + "    ");
            }
        }
    }

}  // namespace

void ListTasksCommand::execute (const SQLiteRepository &repository)
{
    auto tasks = repository.getFullTasks(0, this->show_all ? TaskVisibility::ALL : TaskVisibility::VISIBLE);

    if (tasks.empty())
    {
        std::cout << "Задачи не найдены\n";
    }
    else
    {
        print_tasks(tasks, "");
    }
};
