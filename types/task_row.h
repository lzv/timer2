#pragma once

#include <string>

struct TaskRow
{
    // Все поля в таблице БД NOT NULL. Хоть идентификаторы должны быть беззнаковыми, в БД это знаковые поля.
    long long int id{};
    long long int parent_id{};  // Если 0, то у этой задачи нет родительской задачи.
    long long int created_timestamp{};
    std::string name;
    bool is_hidden{};
};
