#pragma once

#include "command_base.h"

class StopCommand : public CommandBase
{
    long long int task_id{};

  public:
    StopCommand ();
    explicit StopCommand (long long int task_id);

    void execute (const SQLiteRepository &repository) override;
};
