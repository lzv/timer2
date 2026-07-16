#include "connection.h"
#include "sqlite3.h"
#include "../exceptions.h"

SQLiteConnection::SQLiteConnection (const std::string &db_full_filename)
{
    const int flags = static_cast<int>(
        static_cast<unsigned int>(SQLITE_OPEN_READWRITE) | static_cast<unsigned int>(SQLITE_OPEN_CREATE)
    );

    if (sqlite3_open_v2(db_full_filename.c_str(), &db, flags, nullptr) != SQLITE_OK)
    {
        OpenDBError exception_obj{sqlite3_extended_errcode(db), sqlite3_errmsg(db)};
        sqlite3_close(db);
        throw std::move(exception_obj);
    }

    // Для использования соединения должны быть установлены нужные прагмы и созданы таблицы.
    try
    {
        this->checkDB();
    }
    catch (...)
    {
        sqlite3_close(db);
        throw;
    }
}

SQLiteConnection::~SQLiteConnection ()
{
    if (db != nullptr)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}

SQLiteConnection::SQLiteConnection (SQLiteConnection &&other) noexcept : db{other.db}
{
    other.db = nullptr;
}

SQLiteConnection &SQLiteConnection::operator= (SQLiteConnection &&other) noexcept
{
    // Т.к. текущий объект уже существует, у него есть соединение с БД, которое нужно закрыть.
    // У другого объекта должно быть другое соединение, но проверка на всякий случай.
    if (db != nullptr && db != other.db)
    {
        sqlite3_close(db);
    }
    db = other.db;
    other.db = nullptr;
    return *this;
}

void SQLiteConnection::runWriteSql (const std::string &sql, bool use_transaction) const
{
    int ret_code = 0;
    char *err_msg = nullptr;
    const char *tran_sql = nullptr;

    auto make_exception = [] (int ret_code, char *err_msg, const char *sql)
    {
        SqlExecError exception_obj{ret_code, err_msg, sql};
        sqlite3_free(err_msg);
        throw std::move(exception_obj);
    };

    if (use_transaction)
    {
        tran_sql = "BEGIN TRANSACTION;";
        ret_code = sqlite3_exec(db, tran_sql, nullptr, nullptr, &err_msg);

        if (ret_code != SQLITE_OK)
        {
            make_exception(ret_code, err_msg, tran_sql);
        }
    }

    ret_code = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);

    if (ret_code != SQLITE_OK)
    {
        if (use_transaction)
        {
            sqlite3_exec(db, "ROLLBACK TRANSACTION;", nullptr, nullptr, nullptr);
        }
        make_exception(ret_code, err_msg, sql.c_str());
    }

    if (use_transaction)
    {
        tran_sql = "COMMIT TRANSACTION;";
        ret_code = sqlite3_exec(db, tran_sql, nullptr, nullptr, &err_msg);

        if (ret_code != SQLITE_OK)
        {
            sqlite3_exec(db, "ROLLBACK TRANSACTION;", nullptr, nullptr, nullptr);
            make_exception(ret_code, err_msg, tran_sql);
        }
    }
}

SQLiteConnection::RawRows SQLiteConnection::runReadSql (const std::string &sql) const
{
    RawRows result;
    int ret_code = 0;
    char *err_msg = nullptr;

    auto callback = [] (void *param, int cols_count, char **values, char **cols_names) -> int
    {
        if (param == nullptr)
        {
            return 1;
        }
        auto *result = static_cast<RawRows *>(param);

        RawRow row;
        for (int i = 0; i < cols_count; ++i)
        {
            row.try_emplace(cols_names[i], values[i] == nullptr ? std::nullopt : RawCell{values[i]});
        }
        result->push_back(std::move(row));

        return 0;
    };

    ret_code = sqlite3_exec(db, sql.c_str(), callback, &result, &err_msg);

    if (ret_code != SQLITE_OK)
    {
        SqlExecError exception_obj{ret_code, err_msg, sql.c_str()};
        sqlite3_free(err_msg);
        throw std::move(exception_obj);
    }

    return result;
}

long long int SQLiteConnection::affectedRowsCount () const
{
    return sqlite3_changes64(db);
}

long long int SQLiteConnection::lastInsertID () const
{
    return sqlite3_last_insert_rowid(db);
}

void SQLiteConnection::checkDB () const
{
    runWriteSql("PRAGMA foreign_keys = 1;", false);
    runWriteSql("PRAGMA recursive_triggers = 1;", false);
    runWriteSql("PRAGMA secure_delete = 0;", false);

    runWriteSql(
        "CREATE TABLE IF NOT EXISTS tasks ("
        "  id INTEGER PRIMARY KEY,"
        "  parent_id INTEGER DEFAULT NULL REFERENCES tasks(id) ON DELETE CASCADE ON UPDATE CASCADE,"
        "  name TEXT NOT NULL ON CONFLICT ROLLBACK,"
        "  created_timestamp INTEGER DEFAULT (unixepoch()) NOT NULL ON CONFLICT REPLACE,"
        "  is_hidden INTEGER DEFAULT 0 NOT NULL ON CONFLICT REPLACE"
        ") STRICT; "
        "CREATE TABLE IF NOT EXISTS periods ("
        "  id INTEGER PRIMARY KEY,"
        "  task_id INTEGER NOT NULL ON CONFLICT ROLLBACK REFERENCES tasks(id) ON DELETE CASCADE ON UPDATE CASCADE,"
        "  start INTEGER DEFAULT (unixepoch()) NOT NULL ON CONFLICT REPLACE,"
        "  end INTEGER CHECK (end IS NULL OR end > start)"
        ") STRICT; "
        "CREATE INDEX IF NOT EXISTS tasks_parent_id ON tasks(parent_id); "
        "CREATE INDEX IF NOT EXISTS tasks_is_hidden ON tasks(is_hidden); "
        "CREATE INDEX IF NOT EXISTS periods_task_id ON periods(task_id); ",
        true
    );
}
