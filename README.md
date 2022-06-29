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

# Run on Startup

* make sure *Modular_Live_Composer* is installed in *Documents* folder
* `sudo apt-get -y install tmux`
* `nano ~/run_synth_tmux.sh`
* Paste :
```sh
#!/bin/sh
tmux new-session -s "oscar" -d -n "synth"
tmux send-keys -t "oscar:synth" C-z '/home/pi/Documents/Modular_Live_Composer/modular_live_composer' Enter
```
* Close and save `[CTRL] + x`
* `chmod +x ~/run_synth_tmux.sh`
* `sudo nano /etc/rc.local`
* paste `su -c "/home/pi/run_synth_tmux.sh" -s /bin/sh pi` before **exit 0**
* Close and save `[CTRL] + x`

## Stop the program
* `tmux attach`
