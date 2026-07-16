#pragma once

#include "command_base.h"

class StartCommand : public CommandBase
{
    long long int task_id{};

  public:
    explicit StartCommand (long long int task_id);

    void execute (const SQLiteRepository &repository) override;
};
