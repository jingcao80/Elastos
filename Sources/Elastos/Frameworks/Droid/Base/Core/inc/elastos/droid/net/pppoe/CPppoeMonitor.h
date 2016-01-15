
#ifndef __ELASTOS_DROID_NET_PPPOE_CPPPOEMONITOR_H__
#define __ELASTOS_DROID_NET_PPPOE_CPPPOEMONITOR_H__

#include "_Elastos_Droid_Net_Pppoe_CPppoeMonitor.h"
#include "elastos/droid/ext/frameworkext.h"
#include "CPppoeStateTracker.h"

using Elastos::Droid::Net::Pppoe::IPppoeStateTracker;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

CarClass(CPppoeMonitor)
{
public:
    class MyMonitorThread
        : public ThreadBase
    {
    public:
        MyMonitorThread(
          /* [in] */ CPppoeMonitor* owner);

        virtual CARAPI Run();

        virtual CARAPI HandleEvent(
            /* [in] */ const String& ifname,
            /* [in] */ Int32 event);

    private:
        AutoPtr<CPppoeMonitor> mOwner;
    };

    CARAPI StartMonitoring();

    CARAPI constructor(
        /* [in] */ IPppoeStateTracker* tracker);
    CARAPI GetPppoeStateTracker(IPppoeStateTracker** tracker);

private:
    static const String TAG;// = "CPppoeMonitor";
    static const Int32 CONNECTED;// = 1;
    static const Int32 DISCONNECTED;// = 2;
    static const Int32 PHYUP;// = 3;
    static const String connectedEvent;// =    "CONNECTED";
    static const String disconnectedEvent;// = "DISCONNECTED";
    static const Int32 ADD_ADDR;// = 20;
    static const Int32 RM_ADDR;// = 21;
    static const Int32 NEW_LINK;// = 16;
    static const Int32 DEL_LINK;// = 17;
    static const Boolean DEBUG;// = true;
    static const String pppoe_running_flag;// = "net.pppoe.running";
    AutoPtr<IPppoeStateTracker> mTracker;
};

} // Pppoe
} // Net
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_NET_PPPOE_CPPPOEMONITOR_H__
