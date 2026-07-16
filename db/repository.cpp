#include "repository.h"
#include <format>
#include <algorithm>
#include "../utils.h"
#include "connection.h"
#include "../exceptions.h"
#include "../types/task_row.h"
#include "../types/period_row.h"

//------------------------- Локальный код ----------------------------------

// Строки в SQL заключаются в одиночные кавычки, поэтому внутри строки их нужно экранировать.
// Это специализация шаблона std::formatter, которая будет вызвана внутри функции std::format.

struct QuotableStringForSQL : std::string_view
{};

template <>
struct std::formatter<QuotableStringForSQL> : std::formatter<std::string>
{
    auto format (QuotableStringForSQL e, format_context &ctx) const
    {
        std::string result;
        result.reserve(e.length() + 10U);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

        for (const char c : e)
        {
            if (c == '\'')
            {
                result += "\\'";
            }
            else
            {
                result += c;
            }
        }

        return std::formatter<std::string>::format(result, ctx);
    }
};

// Функции, нужные только внутри этого файла, поэтому внутреннее связывание.

namespace
{
    inline void fillTask (TaskRow &task, const SQLiteConnection::RawRow &row)
    {
        // Все поля в таблице БД NOT NULL.
        // NOLINTBEGIN(bugprone-unchecked-optional-access)
        task.id = std::stoll(row.at("id").value());
        task.parent_id = (row.at("parent_id") ? std::stoll(row.at("parent_id").value()) : 0);
        task.created_timestamp = std::stoll(row.at("created_timestamp").value());
        task.name = row.at("name").value();
        task.is_hidden = (row.at("is_hidden").value() != "0");
        // NOLINTEND(bugprone-unchecked-optional-access)
    }

    inline void fillPeriod (PeriodRow &period, const SQLiteConnection::RawRow &row)
    {
        // Поля в таблице БД NOT NULL, кроме end.
        // NOLINTBEGIN(bugprone-unchecked-optional-access)
        period.id = std::stoll(row.at("id").value());
        period.task_id = std::stoll(row.at("task_id").value());
        period.start = std::stoll(row.at("start").value());
        period.end = (row.at("end") ? PeriodRow::EndType{std::stoll(row.at("end").value())} : std::nullopt);
        // NOLINTEND(bugprone-unchecked-optional-access)
    }

    inline void check_id_is_more_than_zero (long long int id, const char *err_message)
    {
        if (id <= 0)
        {
            throw IncorrectSQLArgument{err_message};
        }
    }

    inline void check_id_is_more_or_equal_zero (long long int id, const char *err_message)
    {
        if (id < 0)
        {
            throw IncorrectSQLArgument{err_message};
        }
    }

    enum class ExcType : unsigned char { ARGUMENT, RESULT };

    inline void check_is_not_empty (const auto &obj, const char *err_message, ExcType e_type)
    {
        if (obj.empty())
        {
            if (e_type == ExcType::ARGUMENT)
            {
                throw IncorrectSQLArgument{err_message};
            }

            throw IncorrectSQLResult{err_message};
        }
    }

    inline void check_affected_rows (long long int count, const char *err_message)
    {
        if (count == 0)
        {
            throw IncorrectSQLResult{err_message};
        }
    }

    template <typename T>
    std::string get_where_string_with_numbers (const std::vector<T> &numbers)
    {
        // Не должно вызываться, т.к. такие случаи должны быть проверены до вызова функции.
        if (numbers.empty())
        {
            throw IncorrectSQLArgument{"Ошибка при запросе: не указан перечень id"};
        }

        std::string result;
        result.reserve(numbers.size() * 7);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

        if (numbers.size() == 1)
        {
            result = std::format("= {}", numbers[0]);
        }
        else
        {
            result = std::format("IN ({}", numbers[0]);
            for (std::vector<long long int>::size_type i = 1; i < numbers.size(); ++i)
            {
                result += std::format(", {}", numbers[i]);
            }
            result += ")";
        }

        return result;
    }

}  // namespace

//------------------------- end Локальный код -------------------------------

SQLiteRepository::SQLiteRepository (std::shared_ptr<SQLiteConnection> connection_ptr) :
    connection_ptr{std::move(connection_ptr)}
{}

// Сохраняются только поля "parent_id", "name" и "is_hidden".
long long int SQLiteRepository::addTask (const TaskRow &task) const
{
    check_id_is_more_or_equal_zero(task.parent_id, "Ошибка при добавлении задачи: родительский id меньше 0");
    check_is_not_empty(task.name, "Ошибка при добавлении задачи: не указано название", ExcType::ARGUMENT);

    constexpr const char *sql = R"(INSERT INTO "tasks" ("parent_id", "name", "is_hidden") VALUES ({}, '{}', {});)";

    if (task.parent_id == 0)
    {
        connection_ptr->runWriteSql(
            std::format(sql, "NULL", QuotableStringForSQL{task.name}, task.is_hidden ? 1 : 0), false
        );
    }
    else
    {
        connection_ptr->runWriteSql(
            std::format(sql, task.parent_id, QuotableStringForSQL{task.name}, task.is_hidden ? 1 : 0), false
        );
    }

    return connection_ptr->lastInsertID();
}

TaskRow SQLiteRepository::getTask (long long int id) const
{
    check_id_is_more_than_zero(id, "Ошибка при запросе задачи: id меньше либо равен 0");

    auto rows = connection_ptr->runReadSql(std::format(R"(SELECT * FROM "tasks" WHERE "id" = {};)", id));

    check_is_not_empty(rows, "Ошибка при запросе задачи: задача с указанным id не найдена", ExcType::RESULT);

    // Можно было инициализировать агрегатно с назначенными инициализаторами, но тогда нельзя будет менять их порядок.
    TaskRow result;
    fillTask(result, rows[0]);

    return result;
}

// created_timestamp не обновляется.
void SQLiteRepository::updateTask (const TaskRow &task) const
{
    check_id_is_more_than_zero(task.id, "Ошибка при обновлении задачи: id меньше либо равен 0");
    check_id_is_more_or_equal_zero(task.parent_id, "Ошибка при обновлении задачи: родительский id меньше 0");
    check_is_not_empty(task.name, "Ошибка при обновлении задачи: пустое название", ExcType::ARGUMENT);

    constexpr const char *sql =
        R"(UPDATE "tasks" SET "parent_id" = {}, "name" = '{}', "is_hidden" = {} WHERE "id" = {};)";

    if (task.parent_id == 0)
    {
        connection_ptr->runWriteSql(
            std::format(sql, "NULL", QuotableStringForSQL{task.name}, task.is_hidden ? 1 : 0, task.id), false
        );
    }
    else
    {
        connection_ptr->runWriteSql(
            std::format(sql, task.parent_id, QuotableStringForSQL{task.name}, task.is_hidden ? 1 : 0, task.id), false
        );
    }

    check_affected_rows(
        connection_ptr->affectedRowsCount(), "Ошибка при обновлении задачи: задача с указанным id не найдена"
    );
}

void SQLiteRepository::deleteTask (long long int id) const
{
    check_id_is_more_than_zero(id, "Ошибка при удалении задачи: id меньше либо равен 0");

    connection_ptr->runWriteSql(std::format(R"(DELETE FROM "tasks" WHERE "id" = {};)", id), false);
}

long long int SQLiteRepository::addActivePeriodForTask (long long int task_id) const
{
    check_id_is_more_than_zero(task_id, "Ошибка при добавлении периода: id задачи меньше либо равен 0");

    connection_ptr->runWriteSql(std::format(R"(INSERT INTO "periods" ("task_id") VALUES ({});)", task_id), false);

    return connection_ptr->lastInsertID();
}

PeriodRow SQLiteRepository::getPeriod (long long int id) const
{
    check_id_is_more_than_zero(id, "Ошибка при запросе периода: id меньше либо равен 0");

    auto rows = connection_ptr->runReadSql(std::format(R"(SELECT * FROM "periods" WHERE "id" = {};)", id));

    check_is_not_empty(rows, "Ошибка при запросе периода: период с указанным id не найден", ExcType::RESULT);

    PeriodRow result;
    fillPeriod(result, rows[0]);

    return result;
}

void SQLiteRepository::updatePeriod (const PeriodRow &period) const
{
    check_id_is_more_than_zero(period.id, "Ошибка при обновлении периода: id меньше либо равен 0");
    check_id_is_more_than_zero(period.task_id, "Ошибка при обновлении периода: id задачи меньше либо равен 0");

    constexpr const char *sql = R"(UPDATE "periods" SET "task_id" = {}, "start" = {}, "end" = {} WHERE "id" = {};)";

    if (period.end)
    {
        connection_ptr->runWriteSql(
            std::format(sql, period.task_id, period.start, period.end.value(), period.id), false
        );
    }
    else
    {
        connection_ptr->runWriteSql(std::format(sql, period.task_id, period.start, "NULL", period.id), false);
    }

    check_affected_rows(
        connection_ptr->affectedRowsCount(), "Ошибка при обновлении периода: период с указанным id не найден"
    );
}

void SQLiteRepository::deletePeriod (long long int id) const
{
    check_id_is_more_than_zero(id, "Ошибка при удалении периода: id меньше либо равен 0");

    connection_ptr->runWriteSql(std::format(R"(DELETE FROM "periods" WHERE "id" = {};)", id), false);
}

PeriodsVec SQLiteRepository::getPeriods (long long int task_id) const
{
    check_id_is_more_than_zero(task_id, "Ошибка при запросе периодов: id задачи меньше либо равен 0");

    return this->getPeriods(std::vector<long long int>{task_id});
}

PeriodsVec SQLiteRepository::getPeriods (const std::vector<long long int> &task_ids) const
{
    if (task_ids.empty())
    {
        return {};
    }

    auto rows = connection_ptr->runReadSql(
        std::format(R"(SELECT * FROM "periods" WHERE "task_id" {};)", get_where_string_with_numbers(task_ids))
    );
    PeriodsVec result;

    for (const auto &row : rows)
    {
        result.emplace_back();
        fillPeriod(result.back(), row);
    }

    return result;
}

TasksVec SQLiteRepository::getFullTasks (long long int parent_id, TaskVisibility visible) const
{
    check_id_is_more_or_equal_zero(parent_id, "Ошибка при запросе задач: parent_id меньше 0");

    return this->getFullTasks(std::vector<long long int>{parent_id}, visible);
}

namespace
{
    // Одноразовая функция для сокращения размера другой функции.
    std::string getSQLForFullTasks (const std::vector<long long int> &parent_ids, TaskVisibility visible)
    {
        // Кусочек запроса, если нужно фильтровать по видимости.
        const char *is_hidden = "";
        if (visible == TaskVisibility::VISIBLE)
        {
            is_hidden = R"( AND "is_hidden" = 0)";
        }

        // Если среди родительских id указан 0, нужно запросить задачи верхнего уровня, у которых parent_id = NULL.
        const char *or_is_null = "";
        if (std::ranges::find(parent_ids, 0) != parent_ids.end())
        {
            or_is_null = R"( OR "parent_id" IS NULL)";
        }

        return std::format(
            R"(SELECT * FROM "tasks" WHERE ("parent_id" {}{}){};)",
            get_where_string_with_numbers(parent_ids),
            or_is_null,
            is_hidden
        );
    }

}  // namespace

TasksVec SQLiteRepository::getFullTasks (const std::vector<long long int> &parent_ids, TaskVisibility visible) const
{
    if (parent_ids.empty())
    {
        return {};
    }

    auto rows = connection_ptr->runReadSql(getSQLForFullTasks(parent_ids, visible));

    // Результат запроса пуст.
    if (rows.empty())
    {
        return {};
    }

    TasksVec result;
    std::vector<long long int> tasks_ids;

    // Заполняем основные данные и подготавливаем список id для дополнительных запросов.
    for (const auto &row : rows)
    {
        result.emplace_back();
        fillTask(result.back().data, row);
        tasks_ids.push_back(result.back().data.id);
    }

    // Запрашиваем периоды и размещаем их по задачам.
    for (const PeriodsVec all_periods = this->getPeriods(tasks_ids); const auto &period : all_periods)
    {
        for (auto &task : result)
        {
            if (task.data.id == period.task_id)
            {
                task.periods.push_back(period);  // Тривиально-копируемый объект, std::move() не нужно.
                break;
            }
        }
    }

    // Запрашиваем рекурсивно подзадачи и размещаем их по задачам.
    for (TasksVec sub_tasks = this->getFullTasks(tasks_ids, visible); auto &sub_task : sub_tasks)
    {
        for (auto &task : result)
        {
            if (task.data.id == sub_task.data.parent_id)
            {
                task.sub_tasks.push_back(std::move(sub_task));
                break;
            }
        }
    }

    return result;
}

Task SQLiteRepository::getFullTask (long long int id, TaskVisibility visible) const
{
    check_id_is_more_than_zero(id, "Ошибка при запросе задачи: id меньше либо равен 0");

    Task result;

    result.data = this->getTask(id);
    result.periods = this->getPeriods(id);
    result.sub_tasks = this->getFullTasks(id, visible);

    return result;
}

long long int SQLiteRepository::stopAllTasks () const
{
    connection_ptr->runWriteSql(
        std::format(R"(UPDATE "periods" SET "end" = {} WHERE "end" IS NULL;)", getCurrentTimestamp()), false
    );

    return connection_ptr->affectedRowsCount();
}
