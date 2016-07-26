#!/bin/sh

#Move to setup directory
cd setup/


#General settings
sudo apt-get -y update
sudo apt-get -y install libtool
sudo apt-get -y install autoconf


#Wi-Fi network hosting 
sudo apt-get -y install isc-dhcp-server
sudo apt-get -y install hostapd
sudo echo "ipv6" >> /etc/modules
cp hostapd/hostapd.conf /etc/hostapd/hostapd.conf

#Serial port setup
##libserialport library
git clone git://sigrok.org/libserialport libserialport
cd libserialport/
./autogen.sh
./configure
make
sudo make install
cd ../
##cMultiWii
git clone https://github.com/rgw3d/cMultiWii.git cMultiWii


#PiQuadcopter server setup
##websocketpp
git clone https://github.com/zaphoyd/websocketpp.git websocketpp
##supporting libraries for websocketpp
sudo apt-get -y install libboost-dev
sudo apt-get -y install libboost-system-dev
sudo apt-get -y install libboost-chrono-dev
## ensure compiler can find all linked libraries for when we compile everything
sudo ldconfig


#PiQuadcopter server 
cd ../
make #this will take a while.......

exit 0
