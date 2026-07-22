#pragma once

#include "command_base.h"

class StatCommand : public CommandBase
{
  public:
    enum class TimeEntity : unsigned char { DAY, WEEK, MONTH };

  private:
    TimeEntity time_entity;
    int shift;

  public:
    explicit StatCommand (TimeEntity time_entity = TimeEntity::DAY, int shift = 0);

    void execute (const SQLiteRepository &repository) override;
};
