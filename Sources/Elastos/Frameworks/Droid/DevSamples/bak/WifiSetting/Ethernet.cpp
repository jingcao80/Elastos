
#include "Ethernet.h"
#include <stdlib.h>
#include <stdio.h>
#include <netutils/dhcp.h>
#include <cutils/properties.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <unistd.h>

String Ethernet::sInterfaceEthe = String("eth0");

Boolean Ethernet::RunDhcpEthe()
{
    //StopDhcpEthe();

    int result;
    char  ipaddr[PROPERTY_VALUE_MAX];
    uint32_t prefixLength;
    char gateway[PROPERTY_VALUE_MAX];
    char    dns1[PROPERTY_VALUE_MAX];
    char    dns2[PROPERTY_VALUE_MAX];
    char  server[PROPERTY_VALUE_MAX];
    uint32_t lease;
    char vendorInfo[PROPERTY_VALUE_MAX];

    result = ::dhcp_do_request(
        sInterfaceEthe, ipaddr, gateway,
        &prefixLength, dns1, dns2, server, &lease, vendorInfo);
    property_set("net.dns1", dns1);
    property_set("net.dns2", dns2);
    property_set("net.state", "1");

    return result == 0;
}

Boolean Ethernet::StopDhcpEthe()
{
    property_set("net.dns1", 0);
    property_set("net.dns2", 0);
    property_set("net.state", "0");
    return ::dhcp_stop(sInterfaceEthe) == 0;
}
