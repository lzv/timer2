#pragma once

#include <memory>
#include <vector>
#include "../types/periods_vec.h"
#include "../types/task.h"

class SQLiteConnection;
struct TaskRow;
struct PeriodRow;

enum class TaskVisibility : unsigned char { ALL, VISIBLE };

// Класс взаимодействует между программой и соединением с БД, работая с SQL и с типами сущностей.

class SQLiteRepository
{
    std::shared_ptr<SQLiteConnection> connection_ptr;

  public:
    explicit SQLiteRepository (std::shared_ptr<SQLiteConnection> connection_ptr);

    // Работа с одной задачей.
    long long int addTask (const TaskRow &task) const;  // NOLINT(modernize-use-nodiscard)
    [[nodiscard]] TaskRow getTask (long long int id) const;
    void updateTask (const TaskRow &task) const;
    void deleteTask (long long int id) const;

    // Работа с одним периодом.
    long long int addActivePeriodForTask (long long int task_id) const;  // NOLINT(modernize-use-nodiscard)
    [[nodiscard]] PeriodRow getPeriod (long long int id) const;
    void updatePeriod (const PeriodRow &period) const;
    void deletePeriod (long long int id) const;

    // Работа с множеством периодов.
    [[nodiscard]] PeriodsVec getPeriods (long long int task_id) const;
    [[nodiscard]] PeriodsVec getPeriods (const std::vector<long long int> &task_ids) const;

    // Работа с множеством полных задач, содержащих в себе периоды и подзадачи.
    [[nodiscard]] TasksVec getFullTasks (long long int parent_id, TaskVisibility visible) const;
    [[nodiscard]] TasksVec getFullTasks (const std::vector<long long int> &parent_ids, TaskVisibility visible) const;

    // Запрос одной полной задачи. Параметр visible применяется только к подзадачам.
    [[nodiscard]] Task getFullTask (long long int id, TaskVisibility visible) const;

    // Остановка всех активных задач одним запросом. Возвращается количество измененных периодов.
    long long int stopAllTasks () const;  // NOLINT(modernize-use-nodiscard)
};
