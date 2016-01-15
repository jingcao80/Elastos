
#include "CPppoeMonitor.h"
#include "CPppoeNative.h"
#include "elastos/droid/os/CSystemProperties.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

const String CPppoeMonitor::TAG("CPppoeMonitor");
const Int32 CPppoeMonitor::CONNECTED = 1;
const Int32 CPppoeMonitor::DISCONNECTED = 2;
const Int32 CPppoeMonitor::PHYUP = 3;
const String CPppoeMonitor::connectedEvent("CONNECTED");
const String CPppoeMonitor::disconnectedEvent("DISCONNECTED");
const Int32 CPppoeMonitor::ADD_ADDR = 20;
const Int32 CPppoeMonitor::RM_ADDR = 21;
const Int32 CPppoeMonitor::NEW_LINK = 16;
const Int32 CPppoeMonitor::DEL_LINK = 17;
const Boolean CPppoeMonitor::DEBUG = TRUE;
const String CPppoeMonitor::pppoe_running_flag("net.pppoe.running");

CPppoeMonitor::MyMonitorThread::MyMonitorThread(
    /* [in] */ CPppoeMonitor* owner)
    : mOwner(owner)
{
    Thread::constructor(String("CPppoeMonitor"));
}

ECode CPppoeMonitor::MyMonitorThread::Run()
{
    Int32 index;
    Int32 i;

    //noinspection InfiniteLoopStatement
    for (;;) {
        if (CPppoeMonitor::DEBUG) Slogger::I(TAG, "go poll events");
        String eventName;
        AutoPtr<IPppoeNative> pppoeNative;
        CPppoeNative::AcquireSingleton((IPppoeNative**)&pppoeNative);
        FAIL_RETURN(pppoeNative->WaitForEvent(&eventName));

        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        String propVal;
        sp->Get(mOwner->pppoe_running_flag, &propVal);
        Int32 n = 0;
        if (!propVal.IsNullOrEmpty()) {
            //try
            {
                n = StringUtils::ParseInt32(propVal);
            }
            //catch (NumberFormatException e) {}
        } else {
            if (CPppoeMonitor::DEBUG) Slogger::V(TAG, "%s not FOUND", CPppoeMonitor::pppoe_running_flag.string());
        }

        if (0 == n) {
            if (CPppoeMonitor::DEBUG) Slogger::V(TAG, "! %s. Discard event", CPppoeMonitor::pppoe_running_flag.string());
                continue;
        }

        if (NULL == eventName) {
            continue;
        }
        Slogger::I(TAG, "got event %s", eventName.string());
        /*
         * Map event name into event enum
         */
        AutoPtr< ArrayOf<String> > events;
        FAIL_RETURN(StringUtils::Split(eventName, String(":"), (ArrayOf<String>**)&events));

        index = events->GetLength();
        if (index < 2)
            continue;
        i = 0;
        while (index != 0 && i < index-1) {
            Int32 event = 0;
            Slogger::I(TAG,"dev: %s ev: %s", ((*events)[i]).string(), ((*events)[i+1]).string());
            Int32 cmd = StringUtils::ParseInt32(((*events)[i+1]));
            if ( cmd == DEL_LINK) {
                event = DISCONNECTED;
                HandleEvent((*events)[i], event);
            }
            else if (cmd == ADD_ADDR ) {
                event = CONNECTED;
                HandleEvent((*events)[i], event);
            } else if (cmd == NEW_LINK) {
                event = PHYUP;
                HandleEvent((*events)[i], event);
            }
            i = i + 2;
        }


    }
    return NOERROR;
}

ECode CPppoeMonitor::MyMonitorThread::HandleEvent(
            /* [in] */ const String& ifname,
            /* [in] */ Int32 event)
{
    AutoPtr<IPppoeStateTracker> tracker;
    mOwner->GetPppoeStateTracker((IPppoeStateTracker**)&tracker);

    switch (event) {
        case DISCONNECTED:
            tracker->NotifyStateChange(ifname, NetworkInfoDetailedState_DISCONNECTED);
            break;
        case CONNECTED:
            tracker->NotifyStateChange(ifname, NetworkInfoDetailedState_CONNECTED);
            break;
        case PHYUP:
            tracker->NotifyPppConnected(ifname);
            break;
        default:
            tracker->NotifyStateChange(ifname, NetworkInfoDetailedState_FAILED);
    }
    return NOERROR;
}

ECode CPppoeMonitor::StartMonitoring()
{
        AutoPtr<MyMonitorThread> thread = new MyMonitorThread(this);
        thread->Start();
        return NOERROR;
}

ECode CPppoeMonitor::constructor(
    /* [in] */ IPppoeStateTracker* tracker)
{
   mTracker = tracker;
    return NOERROR;
}

ECode CPppoeMonitor::GetPppoeStateTracker(IPppoeStateTracker** tracker)
{
    VALIDATE_NOT_NULL(tracker);
    *tracker = mTracker;
    REFCOUNT_ADD(*tracker);
    return NOERROR;
}

} // Pppoe
} // Net
} // Droid
} // Elastos

