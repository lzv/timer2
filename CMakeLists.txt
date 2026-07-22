cmake_minimum_required(VERSION 3.12)

project(timer2 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#string(APPEND CMAKE_CXX_FLAGS " -pg")
#string(APPEND CMAKE_EXE_LINKER_FLAGS " -pg")
#string(APPEND CMAKE_SHARED_LINKER_FLAGS " -pg")

add_compile_options(-Wall -Wextra -Wpedantic)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # add_compile_options(-fdiagnostics-all-candidates)
    # add_compile_options(-fmodules-ts)
endif()

set(CMAKE_CXX_CLANG_TIDY "clang-tidy-20;\
--header-filter=.*;\
--exclude-header-filter=sqlite3\\.h"
)

add_executable(target_main 
    main.cpp settings.cpp main_loop.cpp 
    db/connection.cpp db/repository.cpp 
    commands/command_base.cpp commands/command_factory.cpp commands/exit_command.cpp commands/add_task_command.cpp
    commands/help_command.cpp types/periods_vec.cpp types/period_row.cpp types/task.cpp commands/list_tasks_command.cpp
    commands/start_command.cpp commands/stop_command.cpp utils.cpp commands/hide_command.cpp commands/show_command.cpp
    commands/stat_command.cpp
    db/sqlite3.c
)
