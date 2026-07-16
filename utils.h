#pragma once

#include <string>

void loadTZDB ();

long long int getCurrentTimestamp ();

std::string getTimeLength (long long int seconds);
