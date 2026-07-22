#include "period_row.h"
#include "../utils.h"

long long int PeriodRow::length () const
{
    // Если период завершен, возвращается его продолжительность.
    if (this->end)
    {
        return this->end.value() - this->start;
    }

    // В ином случае возвращается продолжительность до текущего момента.
    return getCurrentSysTimestamp() - this->start;
}

bool PeriodRow::is_active () const
{
    // Если указан конец периода, он не активный в текущий момент.
    return !this->end;
}

long long int PeriodRow::getFirstTimePoint () const
{
    return this->start;
}

long long int PeriodRow::getLastTimePoint () const
{
    return this->end ? this->end.value() : getCurrentSysTimestamp();
}

bool PeriodRow::isInsideRange (long long int begin, long long int end) const
{
    return this->getLastTimePoint() > begin && this->getFirstTimePoint() < end;
}

void PeriodRow::crop (long long int begin, long long int end)
{
    if (this->getFirstTimePoint() < begin)
    {
        this->start = begin;
    }

    if (this->getLastTimePoint() > end)
    {
        this->end = end;
    }
}
