#pragma once

#include "command_base.h"

class HideCommand : public CommandBase
{
    long long int task_id{};

  public:
    explicit HideCommand (long long int task_id);

    void execute (const SQLiteRepository &repository) override;
};
