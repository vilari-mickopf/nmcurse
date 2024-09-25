#include <thread>
#include <memory>

#include "ui/network.hpp"


int main() {
    auto ui = std::make_unique<NetworkUi>();

    ui->scan();
    ui->displayNetworks();

    while (true) {
        auto network = ui->selectNetwork();
        if (network == "") {
            break;
        }
        ui->connect(network);
        ui->displayNetworks();
    }

    return 0;
}
