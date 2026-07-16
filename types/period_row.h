#pragma once

#include <optional>

struct PeriodRow
{
    using EndType = std::optional<long long int>;

    // Хоть идентификаторы должны быть беззнаковыми, в БД это знаковые поля.
    long long int id{};
    long long int task_id{};
    long long int start{};
    EndType end;

    // Продолжительность периода. Если конец не указан, продолжительность до текущего момента.
    [[nodiscard]] long long int length () const;

    // Активные периоды - те, у которых не указан конец периода.
    [[nodiscard]] bool is_active () const;

    // Возвращение начальных и конечных точек периода.
    [[nodiscard]] long long int getFirstTimePoint () const;
    [[nodiscard]] long long int getLastTimePoint () const;
};
