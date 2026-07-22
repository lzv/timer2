#pragma once

#include <vector>
#include "period_row.h"  // Можно было бы сделать просто предварительное объявление PeriodRow, и подключать
                         // period_row.h в cpp. Но тогда требуется подключать его в тех местах, где удаляются
                         // элементы вектора, что неочевидно, т.к. period_row.h не используется напрямую.
                         // А также нужно будет определить деструктор вектора в cpp файле.

class PeriodsVec : public std::vector<PeriodRow>
{
    using std::vector<PeriodRow>::vector;

  public:
    // Продолжительность всех периодов в контейнере.
    [[nodiscard]] long long int length_of_all () const;

    // Возвращает true, если хотя бы один период в контейнере активен.
    [[nodiscard]] bool is_active () const;

    // Возвращает начало самого раннего периода.
    [[nodiscard]] long long int getFirstTimePoint () const;

    // Возвращение конца самого позднего периода, или текущего момента для открытого периода.
    [[nodiscard]] long long int getLastTimePoint () const;

    // Обрезает все периоды по указанному диапазону [begin, end].
    // Если период не заходит внутрь диапазона хотя бы на секунду, он удаляется.
    // Активные периоды считаются завершенными в текущий момент.
    // Если текущий момент находится после end, активные периоды завершаются.
    void crop (long long int begin, long long int end);
};
