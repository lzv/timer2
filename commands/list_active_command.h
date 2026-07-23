#pragma once

#include "command_base.h"

class ListActiveCommand : public CommandBase
{
  public:
    void execute (const SQLiteRepository &repository) override;
};
