
#include "elastos/droid/webkit/native/content/browser/TimeZoneMonitor.h"
#include "elastos/droid/webkit/native/content/api/TimeZoneMonitor_dec.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//=====================================================================
//               TimeZoneMonitor::InnerBroadcastReceiver
//=====================================================================
TimeZoneMonitor::InnerBroadcastReceiver::InnerBroadcastReceiver(
    /* [in] */ TimeZoneMonitor* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(TimeZoneMonitor::InnerBroadcastReceiver, Object, IBroadcastReceiver);

ECode TimeZoneMonitor::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (!action.Equals(IIntent::ACTION_TIMEZONE_CHANGED)) {
        Slogger::E(TAG, "unexpected intent");
        return NOERROR;
    }

    return mOwner->NativeTimeZoneChangedFromJava(mOwner->mNativePtr);
}

//=====================================================================
//                           TimeZoneMonitor
//=====================================================================
const String TimeZoneMonitor::TAG("TimeZoneMonitor");

TimeZoneMonitor::TimeZoneMonitor(
    /* [in] */ IContext* context,
    /* [in] */ Int64 nativePtr)
    : mNativePtr(nativePtr)
{
    context->GetApplicationContext((IContext**)&mAppContext);
    AutoPtr<IIntent> stickyIntent;
    mAppContext->RegisterReceiver(mBroadcastReceiver, mFilter, (IIntent**)&stickyIntent);
}

AutoPtr<TimeZoneMonitor> TimeZoneMonitor::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ Int64 nativePtr)
{
    return new TimeZoneMonitor(context, nativePtr);
}

ECode TimeZoneMonitor::Stop()
{
    mAppContext->UnregisterReceiver(mBroadcastReceiver);
    mNativePtr = 0;

    return NOERROR;
}

ECode TimeZoneMonitor::NativeTimeZoneChangedFromJava(
    /* [in] */ Int64 nativeTimeZoneMonitorAndroid)
{
    Elastos_TimeZoneMonitor_nativeTimeZoneChangedFromJava(THIS_PROBE(IInterface), (Handle32)nativeTimeZoneMonitorAndroid);
    return NOERROR;
}

AutoPtr<IInterface> TimeZoneMonitor::GetInstance(
    /* [in] */ IInterface* context,
    /* [in] */ Int64 nativePtr)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    return TO_IINTERFACE(GetInstance(c, nativePtr));
}

void TimeZoneMonitor::Stop(
    /* [in] */ IInterface* obj)
{
    AutoPtr<TimeZoneMonitor> mObj = (TimeZoneMonitor*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "TimeZoneMonitor::Stop, mObj is NULL");
        return;
    }
    mObj->Stop();
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
