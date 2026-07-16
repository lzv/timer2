#pragma once

#include <vector>
#include <string>
#include <memory>

class CommandBase;

std::unique_ptr<CommandBase> getCommand (const std::vector<std::string> &words);
