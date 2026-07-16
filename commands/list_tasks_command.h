#pragma once

#include "command_base.h"

class TasksVec;

class ListTasksCommand : public CommandBase
{
    bool show_all;

  public:
    explicit ListTasksCommand (bool show_all);

    void execute (const SQLiteRepository &repository) override;
};
