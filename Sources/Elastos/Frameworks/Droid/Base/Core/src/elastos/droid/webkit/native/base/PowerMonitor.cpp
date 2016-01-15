
#include "elastos/droid/webkit/native/base/PowerMonitor.h"
#include "elastos/droid/webkit/native/base/api/PowerMonitor_dec.h"
#include "elastos/droid/webkit/native/base/ApplicationState.h"
//#include "elastos/droid/os/CLooper.h"
#include "elastos/droid/os/CLooperHelper.h"

//using Elastos::Droid::Os::CLooper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::ILooperHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

//===============================================================
//                  PowerMonitor::LazyHolder
//===============================================================

//const AutoPtr<PowerMonitor> PowerMonitor::LazyHolder::INSTANCE = new PowerMonitor();

//===============================================================
//                 PowerMonitor::InnerRunnable
//===============================================================
CAR_INTERFACE_IMPL(PowerMonitor::InnerRunnable, Object, IRunnable);

PowerMonitor::InnerRunnable::InnerRunnable()
{
}

ECode PowerMonitor::InnerRunnable::Run()
{
    NativeOnMainActivitySuspended();
    return NOERROR;
}

//===============================================================
//                       PowerMonitor
//===============================================================

const Int64 PowerMonitor::SUSPEND_DELAY_MS;
//AutoPtr<PowerMonitor> PowerMonitor::sInstance;

AutoPtr<IRunnable> PowerMonitor::Runnable_Create()
{
    AutoPtr<IRunnable> runnable = new PowerMonitor::InnerRunnable();
    return runnable;
}

const AutoPtr<IRunnable> PowerMonitor::sSuspendTask = PowerMonitor::Runnable_Create();


static AutoPtr<IHandler> Handler_Create()
{
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    AutoPtr<ILooper> looper;
    helper->GetMainLooper((ILooper**)&looper);
    AutoPtr<IHandler> handler;
    assert(0);
//    CLooper::New(looper, (IHandler**)&handler);
    return handler;
}

PowerMonitor::PowerMonitor()
    : mIsBatteryPower(FALSE)
    , mHandler(Handler_Create())
{
}


void PowerMonitor::CreateForTests(
    /* [in] */ IContext* context)
{
    // Applications will create this once the JNI side has been fully wired up both sides. For
    // tests, we just need native -> java, that is, we don't need to notify java -> native on
    // creation.
    assert(0);
//    sInstance = LazyHolder::INSTANCE;
}

/**
 * Create a PowerMonitor instance if none exists.
 * @param context The context to register broadcast receivers for.  The application context
 *                will be used from this parameter.
 */
void PowerMonitor::Create(
    /* [in] */ IContext* context)
{
    assert(0);
#if 0
    context->GetApplicationContext((IContext**)&context);
    if (sInstance == NULL) {
        sInstance = LazyHolder::INSTANCE;
        ApplicationStatus::RegisterApplicationStateListener(sInstance);
        AutoPtr<IIntentFilter> ifilter;
        CIntentFilter(IIntent::ACTION_BATTERY_CHANGED, (IIntentFilter**)&ifilter);
        AutoPtr<IIntent> batteryStatusIntent;
        context->RegisterReceiver(NULL, ifilter, (IIntent**)&batteryStatusIntent);
        OnBatteryChargingChanged(batteryStatusIntent);
    }
#endif
}

void PowerMonitor::OnBatteryChargingChanged(
    /* [in] */ IIntent* intent)
{
    assert(0);
#if 0
    if (sInstance == NULL) {
        // We may be called by the framework intent-filter before being fully initialized. This
        // is not a problem, since our constructor will check for the state later on.
        return;
    }
    Int32 chargePlug;
    intent->GetIntExtra(IBatteryManager::EXTRA_PLUGGED, -1, &chargePlug);
    // If we're not plugged, assume we're running on battery power.
    sInstance->mIsBatteryPower = chargePlug != IBatteryManager::BATTERY_PLUGGED_USB &&
                                chargePlug != IBatteryManager::BATTERY_PLUGGED_AC;
    nativeOnBatteryChargingChanged();
#endif
}

void PowerMonitor::OnApplicationStateChange(
    /* [in] */ Int32 newState)
{
    if (newState == ApplicationState::HAS_RUNNING_ACTIVITIES) {
        // Remove the callback from the message loop in case it hasn't been executed yet.
        mHandler->RemoveCallbacks(sSuspendTask);
        NativeOnMainActivityResumed();
    }
    else if (newState == ApplicationState::HAS_PAUSED_ACTIVITIES) {
        Boolean result = FALSE;
        mHandler->PostDelayed(sSuspendTask, SUSPEND_DELAY_MS, &result);
    }
}

//@CalledByNative
Boolean PowerMonitor::IsBatteryPower()
{
    assert(0);
//    return sInstance->mIsBatteryPower;
    return FALSE;
}

void PowerMonitor::NativeOnBatteryChargingChanged()
{
    Elastos_PowerMonitor_nativeOnBatteryChargingChanged();
}

void PowerMonitor::NativeOnMainActivitySuspended()
{
    Elastos_PowerMonitor_nativeOnMainActivitySuspended();
}

void PowerMonitor::NativeOnMainActivityResumed()
{
    Elastos_PowerMonitor_nativeOnMainActivityResumed();
}

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
