 #!/bin/bash

#USER="sudo -H -u linuxium"
USER=""
ROOT="sudo"

clear

# pull sources -----------------------------------------------------------------
echo "Updating sources"

$USER git pull
$USER git submodule update --recursive --init
if [ $? -ne 0 ]; then
    echo "git pull failed"
    exit 1
fi
echo "\n"


# build client -----------------------------------------------------------------
echo "Building client"

$USER make clean
#$USER make > build.log
$USER make
if [ $? -ne 0 ]; then
    echo "client build failed"
    exit 1
fi
echo "\n"


# build arduino sketch ---------------------------------------------------------
cd ../arduino
$ROOT ino build
if [ $? -ne 0 ]; then
    echo "avr build failed"
    exit 1
fi

# upload arduino sketch---------------------------------------------------------

# start client -----------------------------------------------------------------
screen -dmS rmc ../client/hello