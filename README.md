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
./nmcurse
```

## AUR

```bash
yay -S nmcurse-git
```


------

<a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg" height="25px"></a>
<a href="https://www.buymeacoffee.com/vilari.mickopf"><img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" height="25px"></a>
