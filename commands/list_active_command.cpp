#include "list_active_command.h"
#include <iostream>
#include <string>
#include <format>
#include "../db/repository.h"
#include "../utils.h"

namespace
{
    void print_tasks (const TasksVec &tasks, const std::string &prefix)
    {
        for (const auto &task : tasks)
        {
            std::cout << std::format(
                "{}[{}{}{}{}] ({}) {}\n",
                prefix,
                task.data.id,
                task.data.is_hidden ? ",H" : "",
                task.is_active() ? ",*" : "",
                task.is_active_self() ? "*" : "",
                getTimeLength(task.getFullLength()),
                task.data.name
            );

            if (!task.sub_tasks.empty())
            {
                print_tasks(task.sub_tasks, prefix + "    ");
            }
        }
    }

}  // namespace

void ListActiveCommand::execute (const SQLiteRepository &repository)
{
    TasksVec tasks = repository.getFullTasks(0, TaskVisibility::ALL);

    auto erase_non_active = [] (auto &self, TasksVec &vec) -> void
    {
        std::erase_if(
            vec,
            [&self] (Task &task) -> bool
            {
                self(self, task.sub_tasks);

                return !task.is_active();
            }
        );
    };

    // Удаляем неактивные задачи.
    erase_non_active(erase_non_active, tasks);

    if (tasks.empty())
    {
        std::cout << "Сейчас нет активных задач\n";
    }
    else
    {
        std::cout << "Сейчас активны следующие задачи:\n\n";
        print_tasks(tasks, "");
    }
}
