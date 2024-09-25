#pragma once

#include <ncurses.h>
#include <string>
#include <atomic>


class Ui {
protected:
    WINDOW* win;
    std::atomic<bool> loading;
    std::atomic<bool> stop;

    void loadingAnimation(const std::string& message);

public:
    Ui(int lines=LINES, int cols=COLS);
    ~Ui();

    void clear();
};
