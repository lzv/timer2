#pragma once

#include <string>

class Settings
{
  public:
    static void setLocate (const char *locale);
    static void setDbFileName (const char *file_name);
    static void setTimeZone (const char *zone);

    static std::string getDbFullFileName ();
    static const char *getTimeZone ();
};
