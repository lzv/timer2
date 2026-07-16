#pragma once

#include "command_base.h"
#include <string>

class AddTaskCommand : public CommandBase
{
    long long int parent_id{};
    std::string name;

  public:
    AddTaskCommand (long long int parent_id, std::string name);

    void execute (const SQLiteRepository &repository) override;
};
