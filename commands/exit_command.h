#pragma once

#include "command_base.h"

class ExitCommand : public CommandBase
{
  public:
    void execute (const SQLiteRepository &repository) override;
};
