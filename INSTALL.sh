#!/bin/bash
# run as su

function checksuccess {
    if [ $? == 0 ]; then
        echo "OK"
    else
        echo "FAIL"
        exit 1
    fi
}


config_dir=$HOME/.config/eyeronic

echo "Copy eyeronic to /usr/sbin/"
sudo cp Release/eyeronic /usr/sbin/eyeronic
checksuccess

if [ ! -d $config_dir ]; then
    echo "Create config directory"
    mkdir $config_dir
    checksuccess
fi

if [ ! -f "$config_dir/config" ]; then
    echo "Copy default config file"
    cp config $config_dir/
    checksuccess
fi

if [ ! -f "$config_dir/coffee.png" ]; then
    echo "Copy default icon"
    cp img/coffee.png $config_dir/
    checksuccess
fi

echo ""
echo "start deamon: eyeronic --config=~/.config/eyeronic/config --start"


