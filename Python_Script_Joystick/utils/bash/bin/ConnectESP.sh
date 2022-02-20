#! /bin/bash

# ------- Bash code to automatically connect ESP device --------- #

echo -n "Waiting for ESP Connection "

# Connect to ESP_B10372, my module SSID, with void password

x='ch';
while [[ $x != '' ]]; do
    nmcli device wifi rescan &> /dev/null

    # Redirect Stderr to tmp.log file
    nmcli dev wifi connect ESP_B10372 password 'ciaomondo' 2> tmp.log 1> /dev/null

    # Read log
    x=$(cat tmp.log)

    for i in {1..3}; do
        echo -n "."
        sleep 0.2
    done
done

rm tmp.log

# Check if really connected to ESP

echo
tmp_var=$(ping -w1 192.168.4.1 2> /dev/null | grep -o " 0%")

if [[ $tmp_var != " 0%" ]]; then
    echo "ESP not connected!"
else
    echo "ESP connected!"
fi
