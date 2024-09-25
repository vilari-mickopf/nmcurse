#include <iostream>
#include <iomanip>
#include <thread>
#include <algorithm>

#include "network.hpp"


NetworkUi::NetworkUi() : Ui(), highlight(0), networks(std::vector<Network>()) {}


void NetworkUi::displayNetworks()
{
    this->getCurrentConnections();

    werase(this->win);

    if (this->networks.size() == 0) {
        wrefresh(win);
        return;
    }

    int max_ssid_len = 3;  // Minimum length is 3 for "---"
    int max_security_len = 3;  // Minimum length is 3 for "---"
    for (const auto& network : this->networks) {
        max_ssid_len = std::max<int>(
            max_ssid_len,
            network.ssid.empty() ? 3 : static_cast<int>(network.ssid.length())
        );

        max_security_len = std::max<int>(
            max_security_len,
            network.security.empty() ? 3 : static_cast<int>(network.security.length())
        );
    }

    /* Calculate the maximum length for SSID + security to fit within the window */
    /* -6 for the spaces left and right */
    size_t max_combined_len = static_cast<size_t>(
        std::max<int>(
            0,
            static_cast<int>(getmaxx(win)) - max_security_len - 6
        )
    );

    /* Calculate the visible range of networks; -4 to account for header and footers */
    size_t win_height = static_cast<size_t>(
        std::max<int>(
            0,
            getmaxy(win) - 4
        )
    );

    size_t start_index = static_cast<size_t>(
        std::max<int>(
            0,
            static_cast<int>(this->highlight) - static_cast<int>(win_height) + 1
        )
    );

    size_t end_index = static_cast<size_t>(
        std::min<int>(
            static_cast<int>(this->networks.size()),
            static_cast<int>(start_index) + static_cast<int>(win_height)
        )
    );

    /* Header */
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, 1, 3, "Available Networks:");
    mvwhline(win, 2, 1, 0, getmaxx(win) - 2);  // Draw a line below the label
    mvwhline(win, getmaxy(win)-1, 1, 0, getmaxx(win) - 2);  // Draw a line below the label

    /* Footer */
    mvwprintw(win, getmaxy(win)-1, 3, "[r: Rescan]");
    wattroff(win, COLOR_PAIR(5));

    unsigned int color;
    for (size_t i=start_index; i < end_index; i++) {
        bool isConnected = std::find( this->connections.begin(),
                                     this->connections.end(),
                                     this->networks[i].ssid)
                                != this->connections.end();

        if (this->networks[i].signal >= 66) {
            color = COLOR_PAIR(3);
        }
        else if (this->networks[i].signal >= 33) {
            color = COLOR_PAIR(2);
        }
        else {
            color = COLOR_PAIR(1);
        }

        std::stringstream ss;
        if (isConnected) {
            color |= WA_BOLD;
            ss << "> ";
        }
        else {
            ss << "  ";
        }

        std::string ssid = this->networks[i].ssid.empty() ?
                                "---" : this->networks[i].ssid;
        std::string security = this->networks[i].security.empty() ?
                                    "---" : this->networks[i].security;

        /* Truncate big SSIDs and add "..." */
        if (ssid.length() > max_combined_len) {
            ssid = ssid.substr(0, max_combined_len - 3) + "...";
        }

        ss << std::left << std::setw(static_cast<int>(max_combined_len)) << ssid
           << std::left << std::setw(max_security_len) << security
           << "  ";

        wattron(win, (i == highlight) ? (color | A_REVERSE) : color);
        /* Adjust position relative to the visible range */
        mvwprintw(win, static_cast<int>(i - start_index + 3), 1, "%s", ss.str().c_str());
        wattroff(win, (i == highlight) ? (color | A_REVERSE) : color);
    }

    wrefresh(win);
}


void NetworkUi::runScan()
{
    this->networks.clear();

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wignored-attributes"
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("nmcli -f SSID,SECURITY,SIGNAL --mode multiline --terse dev wifi list", "r"), pclose);
    #pragma GCC diagnostic pop
    if (!pipe) {
        return;
    }

    char* buffer = nullptr;
    size_t len = 0;

    Network network;
    while (getline(&buffer, &len, pipe.get()) != -1) {
        std::string line(buffer);

        if (!line.empty() && line.back() == '\n') {
            line.erase(line.length() - 1);
        }

        if (line.find("SSID:") == 0) {
            network.ssid = line.substr(5);
        }
        else if (line.find("SECURITY:") == 0) {
            network.security = line.substr(9);
        }
        else if (line.find("SIGNAL:") == 0) {
            try {
                network.signal = std::stoi(line.substr(7));
            } catch (const std::invalid_argument&) {
                network.signal = 0;
            }

            /* Check if network already exists */
            auto it = std::find_if(this->networks.begin(), this->networks.end(),
                [&network](const Network& n) {
                    return n.ssid == network.ssid && n.security == network.security;
                }
            );

            if (it != this->networks.end()) {
                /* Update the signal if the new one is stronger */
                if (network.signal > it->signal) {
                    it->signal = network.signal;
                }
            }
            else {
                /* Add the new network if it doesn't exist */
                this->networks.push_back(network);
            }

            network = Network();
        }
    }
    free(buffer);

    /* Sort the networks by signal strength */
    std::sort(this->networks.begin(), this->networks.end(),
        [](const Network& a, const Network& b) {
            return a.signal > b.signal;
        }
    );
}


void NetworkUi::scan() {
    this->loading = true;
    std::thread loadingThread([this]() {
        this->loadingAnimation("Scanning for networks");
    });

    /* Retry up to 3 times */
    for (int i=0; i < 3; i++) {
        this->runScan();
        if (!this->networks.empty()) {
            break;
        }
    }
    this->loading = false;
    loadingThread.join();

    // if (this->networks.empty()) {
    //     std::cerr << "No networks found or failed to run nmcli." << std::endl;
    // }
}


void NetworkUi::getCurrentConnections()
{
    this->connections.clear();

    std::string cmd = "nmcli -t -f NAME connection show --active 2>/dev/null";
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wignored-attributes"
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    #pragma GCC diagnostic pop
    if (!pipe) {
        return;
    }

    char* buffer = nullptr;
    size_t len = 0;
    std::string line;
    while (getline(&buffer, &len, pipe.get()) != -1) {
        line = buffer;

        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        this->connections.push_back(line);
    }
    free(buffer);
}


int NetworkUi::getInput()
{
    set_escdelay(0);

    int input = wgetch(this->win);
    if (input == ERR) {
        return ERR;
    }

    switch(input) {
        /* Fix for alt/escape/arrows (also f-keys on some terminals) */
        case 27: {
            nodelay(this->win, TRUE);  // Make wgetch non-blocking
            input = wgetch(this->win);
            nodelay(this->win, FALSE); // Restore blocking mode

            if (input == ERR) {
                return 27;
            }

            if (input != 91) {
                return input;
            }

            /* Arrow keys */
            nodelay(this->win, TRUE);  // Make wgetch non-blocking
            input = wgetch(this->win);
            nodelay(this->win, FALSE); // Restore blocking mode
            if (input != ERR) {
                return input;
            }

            return 27;
        }

        default:
            return input;
    }
}


std::string NetworkUi::selectNetwork()
{
    auto input = this->getInput();

    while (input != ERR && input != 13 && input != 'q' && input != 27) {
        if (input == 'r') {
            this->scan();
            highlight = 0;
        }
        else if (highlight > 0 && (input == 65 || input == 'k')) {
            highlight--;
        }
        else if (highlight < this->networks.size()-1 && (input == 66 || input == 'j')) {
            highlight++;
        }

        this->displayNetworks();
        input = this->getInput();
    }

    if (input == ERR || input == 'q' || input == 27) {
        return "";
    }

    return networks[highlight].ssid;
}


bool NetworkUi::isPasswordCached(const std::string& network)
{
    std::string cmd = "nmcli -t -f 802-11-wireless-security.psk connection show ";
    cmd += "'" + network + "' 2>/dev/null";  // Redirect stderr to /dev/null to suppress unwanted messages

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wignored-attributes"
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    #pragma GCC diagnostic pop
    if (!pipe) {
        return false;
    }

    char* buffer = nullptr;
    size_t len = 0;

    std::string result;
    while (getline(&buffer, &len, pipe.get()) != -1) {
        result += buffer;
    }
    free(buffer);

    /* Remove newline characters from result */
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

    return !result.empty();
}

std::string NetworkUi::getPassword()
{
    /* Create a window for the password input */
    int height = 3, width = 50;
    WINDOW* password_win = newwin(height, width,
                                  (LINES - height)/2,
                                  (COLS - width)/2);
    box(password_win, 0, 0);
    mvwprintw(password_win, 1, 1, "Enter Password:");
    wrefresh(password_win);

    /* Capture the password input */
    std::string password;
    int ch;
    while (true) {
        ch = wgetch(password_win);
        if (ch == 13) { /* Enter */
            break;
        }
        else if (ch == 27) { /* ESC */
            password.clear();
            break;
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            /* Handle backspace */
            if (!password.empty()) {
                password.pop_back();

                int x, y;
                getyx(password_win, y, x);
                mvwaddch(password_win, y, x - 1, ' ');
                wmove(password_win, y, x - 1);
            }
        }
        else if (isprint(ch)) {
            password.push_back(static_cast<char>(ch));
            waddch(password_win, '*');
        }
        wrefresh(password_win);
    }

    delwin(password_win);
    return password;
}


void NetworkUi::connect(const std::string& network)
{
    if (network.size() == 0) {
        return;
    }

    std::string password;
    std::string cmd;
    this->clear();
    if (this->isPasswordCached(network)) {
        cmd = "nmcli con up id '" + network + "'";
    }
    else {
        /* Create a new window for password input */
        WINDOW* pass_win = newwin(3, COLS, LINES / 2 - 1, 0);
        werase(pass_win);
        password = this->getPassword();
        delwin(pass_win); /* Delete password input window */

        if (password == "") {
            return;
        }

        cmd = "nmcli dev wifi connect '" + network + "' password '" + password + "'";
    }
    cmd += " >/dev/null 2>&1";  /* Redirect both stdout and stderr to null */
    this->clear();

    this->loading = true;
    std::thread loadingThread([this, network]() {
        this->loadingAnimation("Connecting to network: " + network);
    });
    system(cmd.c_str());
    this->loading = false;
    loadingThread.join();
}
