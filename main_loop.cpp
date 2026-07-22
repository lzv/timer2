#include "main_loop.h"
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "settings.h"
#include "db/connection.h"
#include "db/repository.h"
#include "exceptions.h"
#include "commands/command_base.h"
#include "commands/command_factory.h"

//------------------------- Локальный код ----------------------------------

namespace
{
    void clearTerminal ()
    {
        std::cout << "\033[H\033[J" << std::flush;
    }

    std::vector<std::string> readCommand ()
    {
        std::string input;
        std::string word;
        std::vector<std::string> result;

        std::getline(std::cin, input);
        std::stringstream ss{std::move(input)};

        // Оператор >> очищает строку, безопасно после std::move().
        while (ss >> word)
        {
            result.push_back(std::move(word));
        }

        return result;
    }

}  // namespace

//------------------------- end Локальный код -------------------------------

bool MainLoop::is_started = false;

void MainLoop::start ()
{
    if (is_started)
    {
        return;
    }
    is_started = true;

    // Отключение от БД в деструкторе.
    auto connection = std::make_shared<SQLiteConnection>(Settings::getDbFullFileName());
    const SQLiteRepository repository{connection};

    clearTerminal();

    while (true)
    {
        try
        {
            std::cout << "Введите команду (\"помощь\" для справки): ";
            auto command_words = readCommand();
            clearTerminal();
            auto command = getCommand(command_words);
            if (command)
            {
                command->execute(repository);
            }
            std::cout << "\n";
        }
        catch (const ExceptionWithMessage &exp)
        {
            exp.printMessage();
        }
        catch (ExitException &)
        {
            break;
        }
    }

    is_started = false;
}
