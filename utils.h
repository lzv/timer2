#pragma once

#include <string>
#include <utility>
#include <optional>

void loadTZDB ();

long long int getCurrentSysTimestamp ();

std::string getTimeLength (long long int seconds);

enum class TimeEntity : unsigned char { DAY, WEEK, MONTH };

// Возвращает границы запрошенного периода в unix time (sys time). Конец не включен.
// shift - сдвиг в прошлое. Если отрицательное, будет возвращен будущий период.
// Возвращает пустой std::optional{}, если entity не соответствует какому-либо перечислителю.
std::optional<std::pair<long long int, long long int>> getPeriodBoundaries (TimeEntity entity, int shift);
