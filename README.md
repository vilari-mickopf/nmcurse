# Curses interface for NetworkManager

Just a small ncurses wrapper around nmcli, written in c++

![image](https://github.com/user-attachments/assets/1528f8ee-24ba-4079-b082-bee4bbb8d249)


## Dependencies

- gcc
- make
- cmake
- ncurses


### Arch

```bash
sudo pacman -S gcc make cmake ncurses
```


### Ubuntu/Debian

```bash
sudo pacman -S build-essential cmake libncurses5-dev  # make and gcc are part of build-essential
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
