
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_BATTERYSTATUSMAMAGER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_BATTERYSTATUSMAMAGER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::Build;
// import android.util.Log;

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
 * Android implementation of the battery status APIs.
 */
//@JNINamespace("content")
class BatteryStatusManager
    : public Object
{
private:
  class InnerBroadcastReceiver
    : public Object
    , public IBroadcastReceiver
  {
  public:
    InnerBroadcastReceiver(
      /* [in] */ BatteryStatusManager* owner);

    CARAPI OnReceive(
      /* [in] */ IContext* context,
      /* [in] */ IIntent* intent);

  private:
    BatteryStatusManager* mOwner;
  };

public:
    //@CalledByNative
    static CARAPI_(AutoPtr<BatteryStatusManager>) GetInstance(
      /* [in] */ IContext* appContext);

    /**
     * Start listening for intents
     * @return True on success.
     */
    //@CalledByNative
    CARAPI_(Boolean) Start(
      /* [in] */ Int64 nativePtr);

    /**
     * Stop listening to intents.
     */
    //@CalledByNative
    CARAPI_(void) Stop();

    //@VisibleForTesting
    CARAPI_(void) OnReceive(
      /* [in] */ IIntent* intent);

    static CARAPI_(void*) ElaBatteryStatusManagerCallback_Init();

protected:
    BatteryStatusManager(
      /* [in] */ IContext* context);

    /**
     * Returns whether the BatteryStatusManager should ignore the battery present state.
     * It is required for some devices that incorrectly set the EXTRA_PRESENT property.
     */
    CARAPI_(Boolean) IgnoreBatteryPresentState();

    CARAPI_(void) GotBatteryStatus(
      /* [in] */ Boolean charging,
      /* [in] */ Double chargingTime,
      /* [in] */ Double dischargingTime,
      /* [in] */ Double level);

private:
    static CARAPI_(AutoPtr<IInterface>) GetInstance(
        /* [in] */ IInterface* appContext);

    static CARAPI_(Boolean) Start(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 nativePtr);

    static CARAPI_(void) Stop(
        /* [in] */ IInterface* obj);

    /**
     * Native JNI call
     * see content/browser/battery_status/battery_status_manager.cc
     */
    CARAPI_(void) NativeGotBatteryStatus(
      /* [in] */ Int64 nativeBatteryStatusManager,
      /* [in] */ Boolean charging,
      /* [in] */ Double chargingTime,
      /* [in] */ Double dischargingTime,
      /* [in] */ Double level);

private:
    static const String TAG;

    // A reference to the application context in order to acquire the SensorService.
    const AutoPtr<IContext> mAppContext;
    const AutoPtr<IIntentFilter> mFilter;
    const AutoPtr<IBroadcastReceiver> mReceiver;

    // Non-zero if and only if we're listening for events.
    // To avoid race conditions on the C++ side, access must be synchronized.
    Int64 mNativePtr;

    // The lock to access the mNativePtr.
    const Object mNativePtrLock;

    Boolean mEnabled;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_BATTERYSTATUSMAMAGER_H__
