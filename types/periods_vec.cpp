#include "periods_vec.h"
#include <numeric>
#include <algorithm>
#include <limits>

long long int PeriodsVec::length_of_all () const
{
    return std::accumulate(
        this->begin(),
        this->end(),
        0LL,
        [] (long long int acc, const PeriodRow &row) -> long long int { return acc + row.length(); }
    );
}

[[nodiscard]] bool PeriodsVec::is_active () const
{
    // Вернет false, если вектор пуст.
    return std::ranges::any_of(*this, [] (const PeriodRow &period) -> bool { return period.is_active(); });
}

long long int PeriodsVec::getFirstTimePoint () const
{
    return std::accumulate(
        this->begin(),
        this->end(),
        std::numeric_limits<long long int>::max(),
        [] (long long int acc, const PeriodRow &row) -> long long int { return std::min(acc, row.getFirstTimePoint()); }
    );
}

long long int PeriodsVec::getLastTimePoint () const
{
    return std::accumulate(
        this->begin(),
        this->end(),
        std::numeric_limits<long long int>::min(),
        [] (long long int acc, const PeriodRow &row) -> long long int { return std::max(acc, row.getLastTimePoint()); }
    );
}

void PeriodsVec::crop (long long int begin, long long int end)
{
    std::erase_if(
        *this,
        [begin, end] (PeriodRow &row) -> bool
        {
            if (row.isInsideRange(begin, end))
            {
                row.crop(begin, end);
                return false;
            }
            return true;
        }
    );
}
