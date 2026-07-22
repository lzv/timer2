#include "task.h"
#include <numeric>
#include <algorithm>
#include <limits>

long long int TasksVec::getFullLength () const
{
    return std::accumulate(
        this->begin(),
        this->end(),
        0LL,
        [] (long long int acc, const Task &task) -> long long int { return acc + task.getFullLength(); }
    );
}

long long int TasksVec::getFirstTimePoint () const
{
    return std::accumulate(
        this->begin(),
        this->end(),
        std::numeric_limits<long long int>::max(),
        [] (long long int acc, const Task &task) -> long long int { return std::min(acc, task.getFirstTimePoint()); }
    );
}

long long int TasksVec::getLastTimePoint () const
{
    return std::accumulate(
        this->begin(),
        this->end(),
        std::numeric_limits<long long int>::min(),
        [] (long long int acc, const Task &task) -> long long int { return std::max(acc, task.getLastTimePoint()); }
    );
}

void TasksVec::crop (long long int begin, long long int end)
{
    std::erase_if(
        *this,
        [begin, end] (Task &task) -> bool
        {
            task.periods.crop(begin, end);
            task.sub_tasks.crop(begin, end);

            return task.periods.empty() && task.sub_tasks.empty();
        }
    );
}

bool Task::is_active () const
{
    if (this->periods.is_active())
    {
        return true;
    }

    // Вернет false, если подзадач нет.
    return std::ranges::any_of(this->sub_tasks, [] (const Task &task) -> bool { return task.is_active(); });
}

long long int Task::getFullLength () const
{
    return this->periods.length_of_all() + this->sub_tasks.getFullLength();
}

long long int Task::getFirstTimePoint () const
{
    return std::min(this->periods.getFirstTimePoint(), this->sub_tasks.getFirstTimePoint());
}

long long int Task::getLastTimePoint () const
{
    return std::max(this->periods.getLastTimePoint(), this->sub_tasks.getLastTimePoint());
}
