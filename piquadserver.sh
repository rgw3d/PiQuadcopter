#!/bin/sh 
#
#
# Run the wifi host setup
sudo rm -rf /var/run/hostapd
sudo hostapd -B /etc/hostapd/hostapd.conf
sudo ifconfig wlan0 192.168.10.1
sudo service isc-dhcp-server restart

sudo ./websocket

exit 0
