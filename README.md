<div align="center">
  <h1>livefetch</h1>

  <p>A TUI program like fastfetch that actively updates the shown system information.<br>
</div>

---
## Features

- Live-updating system information.
- Displays common system modules such as:
  - Operating system
  - Kernel
  - CPU
  - GPU
  - Memory
  - Swap
  - Disk usage
  - Local IP
  - Uptime
  - etc.
- Configurable modules and layout.
- Custom ASCII logos.
- Lightweight and written in C.

## Installation
Installation with homebrew
```sh
brew tap WilGulf/livefetch
brew trust WilGulf/livefetch
brew install livefetch
```
Installation from source code<br>
```sh
# Compile the program
make
# Install the program
sudo make install
```
> **Note:** If `PREFIX` is not specified, `make install` installs to `/usr/local` by default.

