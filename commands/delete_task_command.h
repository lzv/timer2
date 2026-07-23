#pragma once

#include "command_base.h"

class DeleteTaskCommand : public CommandBase
{
    long long int id{};

  public:
    explicit DeleteTaskCommand (long long int id);

    void execute (const SQLiteRepository &repository) override;
};