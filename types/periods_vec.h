#pragma once

#include <vector>

struct PeriodRow;

// NOLINTNEXTLINE(hicpp-special-member-functions,cppcoreguidelines-special-member-functions)
class PeriodsVec : public std::vector<PeriodRow>
{
    using std::vector<PeriodRow>::vector;

  public:
    ~PeriodsVec ();

    // Продолжительность всех периодов в контейнере.
    [[nodiscard]] long long int length_of_all () const;

    // Возвращает true, если хотя бы один период в контейнере активен.
    [[nodiscard]] bool is_active () const;

    // Возвращает начало самого раннего периода.
    [[nodiscard]] long long int getFirstTimePoint () const;

    // Возвращение конца самого позднего периода, или текущего момента для открытого периода.
    [[nodiscard]] long long int getLastTimePoint () const;
};
