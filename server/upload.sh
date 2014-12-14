#!/bin/bash

file_list="./ww.php ./probe_data.php"
host="root@homesrv"
options="-v -i /cygdrive/c/users/ww/.ssh/id_rsa"

scp $options $file_list $host:/var/www