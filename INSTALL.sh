#!/bin/bash
# run as su

config_dir=$HOME/.config/eyeronic

sudo cp Release/eyeronic /usr/sbin/eyeronic

if [ ! -d $config_dir ]; then
    mkdir $config_dir
fi

if [ ! -f "$config_dir/config" ]; then
    cp config $config_dir/
fi

if [ ! -f "$config_dir/coffee.png" ]; then
    cp img/coffee.png $config_dir/
fi

echo "start deamon: eyeronic --config=~/.config/eyeronic/config --start"


