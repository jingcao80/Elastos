
#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#include <elastos.h>

using namespace Elastos;

/**
 * Native calls for sending requests to the supplicant daemon, and for
 * receiving asynchronous events. All methods of the form "xxxxCommand()"
 * must be single-threaded, to avoid requests and responses initiated
 * from multiple threads from being intermingled.
 * <p/>
 * Note that methods whose names are not of the form "xxxCommand()" do
 * not talk to the supplicant daemon.
 * Also, note that all WifiNative calls should happen in the
 * WifiStateTracker class except for waitForEvent() call which is
 * on a separate monitor channel for WifiMonitor
 *
 * {@hide}
 */
class Ethernet
{
public:
    static Boolean RunDhcpEthe();

    static Boolean StopDhcpEthe();

public:
    static String sInterfaceEthe;
};

#endif //__ETHERNET_H__
