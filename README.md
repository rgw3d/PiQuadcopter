# PiQuadcopter
Websocket and MultiWii serial code to control a quadcopter, running on a Raspberry Pi

## Overview of all the tech

- The first thing to do is to create a hosted Wi-Fi network. To accomplish this we use [`isc-dhcp-server`](https://help.ubuntu.com/community/isc-dhcp-server) and [`hostapd`](https://w1.fi/hostapd/)
  - `isc-dhcp-server` serves as the DHCP server for our Wi-Fi network
  - `hostapd` serves as access point management, essentially making a Wi-Fi card into a router
- Now for communication with our MultiWii device. I have a [C library](https://github.com/rgw3d/cMultiWii) with the [MSP protocol](http://www.multiwii.com/wiki/index.php?title=Multiwii_Serial_Protocol), and I use a backbone library [`libserialport`](https://sigrok.org/wiki/Libserialport)
  - `libserialport` is downloaded from their git repository, and installed
  - `cMultiWii`, my MSP implementation is downloaded from the git repository. This will be compiled later
- Websocket communication is needed next. I use the [`websocketpp`](https://github.com/zaphoyd/websocketpp) library, implemented in this repo.
  - `websocketpp` is not installed. It is a header-only library (.hpp), and is compiled each time (with the intention of improving performance)
- Compiling websocket.cpp in this repo will build the executable that will recieve commands from a remote control, and then relay them to the MultiWii. It compiles the `websocketpp` library which takes along time, and the `cMultiWii` library which is quick to compile.



## Instructions to set up your Pi with all of the software it needs:

- Download/clone this repo
- Run `setup_pi_software.sh`, which will install `libserialport`, `isc-dhcp-server`, `hostapd`, and download `cMultiWii`, `websocketpp`. It will also compile the websocket server
  - You need to check to see if your MultiWii is `/dev/ttyUSB0`. Check the troubleshooting section for more information
- Since your network configuration might be different, your `/etc/network/interfaces` file is not modified, but needs to be
- Add the following configuration to your `/etc/network/interfaces` file in order for `hostapd` to work properly

```
allow-hotplug wlan0
iface wlan0 inet static
  address 192.168.10.1
  netmask 255.255.255.0
  gateway 192.168.10.1
```
- You need to ensure `wlan0` and `hostapd.conf` are correct. check the troubleshooting seciton for more information
- To setup the network and run the server, run `PiQuadcopter.sh`
  - The above file contains what is needed to startup the hosted network, and will run the websocket server

## Troubleshooting

- To find your wireless interface, run `ifconfig`
- To find your USB device, run `dmesg | grep ttyUSB`  and look for what number it is
  - Once you find your device, you will need to edit line 4 of `configure_port.c` within `cMultiWii` to reflect the proper device.
- If your wireless interface is not `wlan0`, then you will have to change it in your `interfaces` file, and in `hostapd.conf`
- Check `/etc/hostapd/hostapd.conf` for hostapd configurations, and make sure they are correct
