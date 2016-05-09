setprop dhcp.eth0.reason BOUND 
setprop dhcp.eth0.result ok 
ifconfig eth0 192.168.5.249 netmask 255.255.248.0
route add default gw 192.168.0.1 dev eth0 
setprop net.eth0.dns1 8.8.8.8
setprop net.eth0.dns2 210.22.70.3

setprop dhcp.eth0.reason BOUND 
setprop dhcp.eth0.result ok 
ifconfig eth0 10.198.1.186 netmask 255.255.255.255
route add default gw 10.198.1.1 dev eth0 
setprop net.eth0.dns1 8.8.8.8
setprop net.eth0.dns2 210.22.70.3



