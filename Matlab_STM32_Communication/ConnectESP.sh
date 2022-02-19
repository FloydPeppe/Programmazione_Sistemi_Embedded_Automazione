#! /bin/bash

# ------- Bash code to automatically connect ESP device --------- #

echo -n "Waiting for ESP Connection"


# Connect to ESP_B10372, my module SSID, with void password

x='ch';
while [[ $x != '' ]]; do
    nmcli device wifi rescan &> /dev/null
    # Redirect Stderr to tmp.log file
    nmcli dev wifi connect ESP_B10372 password '' 2> tmp.log 1> /dev/null
    x=$(cat tmp.log)
    echo -n "."
    sleep 0.5
done


# Check if really connected to ESP

echo
tmp_var=$(ping -w1 192.168.4.1 2> /dev/null | grep -o " 0%")

if [[ $tmp_var != " 0%" ]]; then
    echo "ESP not connected!" > tmp.log
else
    echo "ESP connected!" > tmp.log
fi
