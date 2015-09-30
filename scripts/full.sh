 #!/bin/bash

USER="sudo -H -u linuxium"
ROOT="sudo"

# pull sources -----------------------------------------------------------------
$USER git pull
if [ $? -ne 0 ]; then
    echo "git pull failed"
    exit 1
fi

# build client -----------------------------------------------------------------
cd ../client
$USER make clean
$USER make
if [ $? -ne 0 ]; then
    echo "client build failed"
    exit 1
fi

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
