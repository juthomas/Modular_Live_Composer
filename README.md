# Install dependencies

## Dependency for SPI :

* `sudo apt-get install cmake`

* [spidev-lib](https://github.com/juthomas/spidev-lib)

* don't forget to unable SPI in `raspi-config` raspberry command

## Package for Midi USB

* `sudo apt-get install libportmidi-dev`

## Install ncurses

* `sudo apt-get install libncurses5-dev libncursesw5-dev`

# Run 

* `make`

* `./modular_live_composer`

# Debug

## debugging using Vs Code

* add `@id:ms-vscode.makefile-tools` extension + add `-g` flag in makefile
