#include "exit_command.h"

#include "../exceptions.h"

void ExitCommand::execute ([[maybe_unused]] const SQLiteRepository &repository)
{
    throw ExitException{""};
}
