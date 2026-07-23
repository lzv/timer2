#pragma once

#include <vector>
#include "task_row.h"
#include "periods_vec.h"

class Task;

class TasksVec : public std::vector<Task>
{
    using std::vector<Task>::vector;

  public:
    // Возвращает суммарную продолжительность работы всех задач и их подзадач.
    [[nodiscard]] long long int getFullLength () const;

    // Возвращает самое ранне начало работы среди всех задач и их подзадач.
    [[nodiscard]] long long int getFirstTimePoint () const;

    // Возвращает самое позднее окончание работы среди всех задач и их подзадач,
    // либо текущий момент, если работа продолжается.
    [[nodiscard]] long long int getLastTimePoint () const;

    // Обрезает все периоды всех задач по указанному диапазону.
    // Если у задачи и ее подзадач не осталось периодов, задача удаляется.
    void crop (long long int begin, long long int end);
};

class Task
{
  public:
    TaskRow data;
    PeriodsVec periods;
    TasksVec sub_tasks;

    // Задача активна, если у нее или ее подзадач есть активный период.
    [[nodiscard]] bool is_active () const;

    // Возвращает true, если активна сама задача, без учета подзадач, иначе возвращает false.
    [[nodiscard]] bool is_active_self () const;

    // Суммарная продолжительность работы над задачей и всеми подзадачами.
    [[nodiscard]] long long int getFullLength () const;

    // Возвращает самое ранне начало работы над задачей или подзадачей.
    [[nodiscard]] long long int getFirstTimePoint () const;

    // Возвращает самое позднее окончание работы над задачей или подзадачей,
    // либо текущий момент, если работа продолжается.
    [[nodiscard]] long long int getLastTimePoint () const;
};
