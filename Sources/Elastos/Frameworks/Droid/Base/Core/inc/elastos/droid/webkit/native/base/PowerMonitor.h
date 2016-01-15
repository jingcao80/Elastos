
#ifndef __ELASTOS_DROID_WEBKIT_BASE_POWERMONITOR_H__
#define __ELASTOS_DROID_WEBKIT_BASE_POWERMONITOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/base/ApplicationStatus.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

/**
 * Integrates native PowerMonitor with the java side.
 */
//@JNINamespace("base::android")
class PowerMonitor
    : public ApplicationStatus::ApplicationStateListener
{
private:
    static AutoPtr<IRunnable> Runnable_Create();
public:
    class LazyHolder
    {
    private:
//        static const AutoPr<PowerMonitor> INSTANCE;
    };

    class InnerRunnable
        : public Object
        , public IRunnable
    {
    public:
        InnerRunnable();

        CAR_INTERFACE_DECL();

        CARAPI Run();
    };

public:
    static CARAPI_(void) CreateForTests(
        /* [in] */ IContext* context);

    /**
     * Create a PowerMonitor instance if none exists.
     * @param context The context to register broadcast receivers for.  The application context
     *                will be used from this parameter.
     */
    static CARAPI_(void) Create(
        /* [in] */ IContext* context);

    static CARAPI_(void) OnBatteryChargingChanged(
        /* [in] */ IIntent* intent);

    //@Override
    CARAPI_(void) OnApplicationStateChange(
        /* [in] */ Int32 newState);

    static CARAPI_(void*) ElaPowerMonitorCallback_Init();

private:
    PowerMonitor();

    //@CalledByNative
    static CARAPI_(Boolean) IsBatteryPower();

    static CARAPI_(void) NativeOnBatteryChargingChanged();
    static CARAPI_(void) NativeOnMainActivitySuspended();
    static CARAPI_(void) NativeOnMainActivityResumed();

private:
    static const Int64 SUSPEND_DELAY_MS = 1 * 60 * 1000;  // 1 minute.

//    static AutoPr<PowerMonitor> sInstance;

    Boolean mIsBatteryPower;
    const AutoPtr<IHandler> mHandler;

    // Asynchronous task used to fire the "paused" event to the native side 1 minute after the main
    // activity transitioned to the "paused" state. This event is not sent immediately because it
    // would be too aggressive. An Android activity can be in the "paused" state quite often. This
    // can happen when a dialog window shows up for instance.
    static const AutoPtr<IRunnable> sSuspendTask;
};

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_BASE_POWERMONITOR_H__
