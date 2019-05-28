#!/bin/bash
# run as su

config_dir=$HOME/.config/eyeronic

sudo cp Release/eyeronic /usr/sbin/eyeronic

if [ ! -d $config_dir ]; then
    mkdir $config_dir
fi

cp config $config_dir/
cp img/coffee.png $config_dir/

echo "start deamon: eyeronic --config=~/.config/eyeronic/config --start"


