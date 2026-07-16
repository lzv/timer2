#include "help_command.h"
#include <iostream>

HelpCommand::HelpCommand (const char *message) : message{message} {}

void HelpCommand::execute ([[maybe_unused]] const SQLiteRepository &repository)
{
    std::cout << message << "\n" << std::flush;
};
