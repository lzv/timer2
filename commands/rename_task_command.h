#pragma once

#include "command_base.h"
#include <string>

class RenameTaskCommand : public CommandBase
{
    long long int id{};
    std::string new_name;

  public:
    explicit RenameTaskCommand (long long int id, std::string new_name);

    void execute (const SQLiteRepository &repository) override;
};
