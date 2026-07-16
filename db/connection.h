#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

struct sqlite3;

// Класс отвечает за соединение с БД и непосредственное выполнение SQL-запросов.
// Соединение устанавливается в конструкторе и закрывается в деструкторе, поэтому объект нельзя копировать
// (при необходимости копировать можно сделать подсчет ссылок, но std::shared_ptr справится лучше).

class SQLiteConnection
{
  public:
    using RawCell = std::optional<std::string>;
    using RawRow = std::map<std::string, RawCell, std::less<>>;
    using RawRows = std::vector<RawRow>;

  private:
    sqlite3 *db = nullptr;

    // Должен быть вызван сразу после открытия соединения с БД.
    void checkDB () const;

  public:
    // Копировать объект нельзя
    SQLiteConnection (const SQLiteConnection &) = delete;
    SQLiteConnection &operator= (const SQLiteConnection &) = delete;

    // Перемещать объект можно
    SQLiteConnection (SQLiteConnection &&other) noexcept;
    SQLiteConnection &operator= (SQLiteConnection &&other) noexcept;

    // В конструкторе сразу подключение к БД, с созданием файла БД при необходимости
    explicit SQLiteConnection (const std::string &db_full_filename);

    // В деструкторе отключение от БД
    ~SQLiteConnection ();

    //-------------------------------------------------------------------------

    // Выполнение SQL, который изменяет БД.
    void runWriteSql (const std::string &sql, bool use_transaction) const;

    // Количество строк, затронутых последним запросом INSERT, UPDATE или DELETE
    // (только в основной таблице, без учета триггеров).
    [[nodiscard]] long long int affectedRowsCount () const;

    // id последней вставленной записи в таблицу с rowid (не учитывая вставки при срабатывании триггеров).
    [[nodiscard]] long long int lastInsertID () const;

    // Выполнение SQL, который только читает данные.
    [[nodiscard]] RawRows runReadSql (const std::string &sql) const;
};
