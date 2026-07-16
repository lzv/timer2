#pragma once

#include <stdexcept>
#include <format>
#include <iostream>

// Через это исключение производится выход из бексонечного цикла обработки команд.
class ExitException : public std::runtime_error
{
    using runtime_error::runtime_error;  // Наследуется уровень доступа конструкторов в родительском классе
};

class ExceptionWithMessage : public std::runtime_error
{
    using runtime_error::runtime_error;  // Наследуется уровень доступа конструкторов в родительском классе

  public:
    void printMessage () const
    {
        std::cout << this->what() << '\n' << std::flush;
    }
};

class SetLocaleError : public ExceptionWithMessage
{
  public:
    explicit SetLocaleError (const char *mess) :
        ExceptionWithMessage{std::format("Ошибка при установке локали: {}", mess)}
    {}
};

class GetCurrentPathError : public ExceptionWithMessage
{
  public:
    explicit GetCurrentPathError (int code) :
        ExceptionWithMessage{std::format("Ошибка при получении текущего каталога: код ошибки {}", code)}
    {}
};

class OpenDBError : public ExceptionWithMessage
{
  public:
    OpenDBError (int ex_err_code, const char *err_msg) :
        ExceptionWithMessage{
            std::format("Ошибка при открытии или создании БД (расширенный код ошибки {}): {}", ex_err_code, err_msg)
        }
    {}
};

class SqlExecError : public ExceptionWithMessage
{
  public:
    SqlExecError (int err_code, const char *err_message, const char *sql) :
        ExceptionWithMessage{std::format(
            "При выполнении SQL запроса:\n\n{}\n\nпроизошла ошибка (код {}): {}", sql, err_code, err_message
        )}
    {}
};

// Когда SQL выполняется без ошибок, но результат не тот, который ожидается.
// Это исключение должно перехватываться без завершения программы, как часть обработки команды.
class IncorrectSQLResult : public ExceptionWithMessage
{
  public:
    explicit IncorrectSQLResult (const char *message) : ExceptionWithMessage{message} {}
};

// Когда для SQL запроса переданы неверные данные. Также перехватывается как часть обработки команды.
class IncorrectSQLArgument : public ExceptionWithMessage
{
  public:
    explicit IncorrectSQLArgument (const char *message) : ExceptionWithMessage{message} {}
};

// Ошибка в аргументах команды или отсутствующая команда.
class CommandError : public ExceptionWithMessage
{
  public:
    explicit CommandError (const char *message) : ExceptionWithMessage{message} {}
};
