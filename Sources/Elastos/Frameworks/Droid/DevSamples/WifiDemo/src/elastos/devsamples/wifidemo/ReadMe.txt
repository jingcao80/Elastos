Before text the Wifi code, we can use the wpa_cli/wpa_supplicant to test the driver and hardware.

example:
wpa_supplicant -Dnl80211 -iwlan0 -c/data/misc/wifi/wpa_supplicant.conf -p/data/misc/wifi/sockets

wpa_cli -iwlan0 -p/data/misc/wifi/sockets
