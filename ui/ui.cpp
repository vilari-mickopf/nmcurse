#include "ui.hpp"

#include <thread>


Ui::Ui(int lines, int cols) : loading(false), stop(false)
{
    setlocale(LC_ALL, "");
    initscr();
    start_color();

    // Define color pairs for different signal levels
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    curs_set(0);

    noecho();
    nonl();
    raw();

    start_color();
    use_default_colors();

    // cbreak();
    // keypad(stdscr, TRUE);
    this->win = newwin(lines, cols, 0, 0);
}


void Ui::clear()
{
    werase(this->win);
    wrefresh(this->win);
}


void Ui::loadingAnimation(const std::string& message)
{
    const char* spinner = "|/-\\";
    int i = 0;

    while (this->loading) {
        /* Get the current terminal size */
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        /* Adjust window size and position if needed */
        wresize(this->win, max_y, max_x);
        mvwin(this->win, 0, 0);

        /* Clear the window */
        this->clear();
        refresh();  // Ensures the terminal is fully updated

        /* Draw the message and spinner at a fixed position */
        wattron(win, COLOR_PAIR(5));
        mvwprintw(this->win, 1, 1, "%s %c", message.c_str(), spinner[i % 4]);
        mvwhline(win, 2, 1, 0, getmaxx(win) - 2);  // Draw a line below the label
        wattroff(win, COLOR_PAIR(5));

        /* Refresh the window to show the updated content */
        wrefresh(this->win);

        /* Update spinner index and sleep */
        i++;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

Ui::~Ui() {
    endwin();
}
