#pragma once

#include "command_base.h"

// Просто выводит строку, переданную в конструктор.

class HelpCommand : public CommandBase
{
    const char *message;

  public:
    explicit HelpCommand (const char *message);

    void execute (const SQLiteRepository &repository) override;
};
