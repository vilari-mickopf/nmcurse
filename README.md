# Curses interface for NetworkManager

Just a small ncurses wrapper around nmcli, written in c++

![image](https://github.com/user-attachments/assets/1528f8ee-24ba-4079-b082-bee4bbb8d249)


## Dependencies

- gcc
- make
- cmake
- ncurses
- NetworkManager


### Arch

```bash
sudo pacman -S gcc make cmake ncurses networkmanager
```


### Ubuntu/Debian

```bash
sudo apt install build-essential cmake libncurses5-dev network-manager  # make and gcc are part of build-essential
```

Enable and start network manager if it's not running already
```bash
sudo systemctl enable --now NetworkManager
```


## Build

```bash
mkdir build && cd build
cmake ..
make -j
sudo make install
```


## Run

```bash
nmcurse
```

------

<a href="https://opensource.org/licenses/MIT" rel="nofollow"><img src="https://camo.githubusercontent.com/a4426cbe5c21edb002526331c7a8fbfa089e84a550567b02a0d829a98b136ad0/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f4c6963656e73652d4d49542d79656c6c6f772e737667" alt="License: MIT" data-canonical-src="https://img.shields.io/badge/License-MIT-yellow.svg" height="25px"></a>
<a href="https://www.buymeacoffee.com/vilari.mickopf"><img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" height="25px"></a>
