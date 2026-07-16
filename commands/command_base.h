#pragma once

class SQLiteRepository;

class CommandBase
{
  public:
    CommandBase () = default;
    CommandBase (const CommandBase &) = default;
    CommandBase (CommandBase &&) = default;
    CommandBase &operator= (const CommandBase &) = default;
    CommandBase &operator= (CommandBase &&) = default;

    virtual void execute (const SQLiteRepository &repository) = 0;

    virtual ~CommandBase () = default;
};
