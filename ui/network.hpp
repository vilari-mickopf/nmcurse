#pragma once

#include "ui.hpp"

#include <string>
#include <vector>


struct Network {
    std::string ssid;
    std::string security;
    int signal;
};


class NetworkUi : Ui {
private:
    size_t highlight;
    std::vector<Network> networks;
    std::vector<std::string> connections;

    int getInput();

    void runScan();
    void getCurrentConnections();
    bool isPasswordCached(const std::string& network);
    std::string getPassword();

public:
    NetworkUi();

    void scan();
    void displayNetworks();
    std::string selectNetwork();
    void connect(const std::string& network);
};
