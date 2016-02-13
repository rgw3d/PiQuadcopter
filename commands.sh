sudo echo "ipv6" >> /etc/modules
sudo apt-get -y install isc-dhcp-server
sudo apt-get -y install hostapd
sudo apt-get -y install libboost-dev
sudo apt-get -y install libboost-system-dev
sudo apt-get -y install libboost-chrono-dev
g++ testweb.cpp -I /home/pi/Desktop/websocketpp/ -lboost_system -lboost_chrono -lpthread
git clone https://github.com/crayzeewulf/libserial.git libserial
libtoolize
commands.txt
cat commands.txt
sudo apt-get install isc-dhcp-server
sudo apt-get install hostapd
sudo apt-get install libboost-dev
sudo apt-get install libboost-system-dev
sudo apt-get install libboost-chrono-dev

sudo apt-get install libtool
sudo ldconfig
