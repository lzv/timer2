#pragma once

#include "command_base.h"

class ShowCommand : public CommandBase
{
    long long int task_id{};

  public:
    explicit ShowCommand (long long int task_id);

    void execute (const SQLiteRepository &repository) override;
};
