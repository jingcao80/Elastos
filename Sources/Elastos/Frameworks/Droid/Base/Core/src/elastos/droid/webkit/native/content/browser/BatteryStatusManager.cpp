
#include "elastos/droid/webkit/native/content/browser/BatteryStatusManager.h"
#include "elastos/droid/webkit/native/content/api/BatteryStatusManager_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//===============================================================
//         BatteryStatusManager::InnerBroadcastReceiver
//===============================================================

BatteryStatusManager::InnerBroadcastReceiver::InnerBroadcastReceiver(
  /* [in] */ BatteryStatusManager* owner)
  : mOwner(owner)
{
}

ECode BatteryStatusManager::InnerBroadcastReceiver::OnReceive(
  /* [in] */ IContext* context,
  /* [in] */ IIntent* intent)
{
  mOwner->OnReceive(intent);
  return NOERROR;
}

//===============================================================
//                   BatteryStatusManager
//===============================================================

const String BatteryStatusManager::TAG("BatteryStatusManager");

BatteryStatusManager::BatteryStatusManager(
  /* [in] */ IContext* context)
  : mNativePtr(0)
  , mEnabled(FALSE)
{
  assert(0);
  // mReceiver = new InnerBroadcastReceiver(this);
  // CIntentFilter::New(IIntent::ACTION_BATTERY_CHANGED, (IIntentFilter**)&mFilter);
  // context->GetApplicationContext((IContext**)&mAppContext);
}

//@CalledByNative
AutoPtr<BatteryStatusManager> BatteryStatusManager::GetInstance(
  /* [in] */ IContext* appContext)
{
    return new BatteryStatusManager(appContext);
}

/**
 * Start listening for intents
 * @return True on success.
 */
//@CalledByNative
Boolean BatteryStatusManager::Start(
  /* [in] */ Int64 nativePtr)
{
  assert(0);
    // synchronized(mNativePtrLock) {
    //     if (!mEnabled && mAppContext.registerReceiver(mReceiver, mFilter) != null) {
    //         // success
    //         mNativePtr = nativePtr;
    //         mEnabled = true;
    //     }
    // }
    // return mEnabled;
  return FALSE;
}

/**
 * Stop listening to intents.
 */
//@CalledByNative
void BatteryStatusManager::Stop()
{
  assert(0);
    // synchronized(mNativePtrLock) {
    //     if (mEnabled) {
    //         mAppContext.unregisterReceiver(mReceiver);
    //         mNativePtr = 0;
    //         mEnabled = false;
    //     }
    // }
}

//@VisibleForTesting
void BatteryStatusManager::OnReceive(
  /* [in] */ IIntent* intent)
{
  assert(0);

#if 0
   if (!intent.getAction().equals(Intent.ACTION_BATTERY_CHANGED)) {
       Log.e(TAG, "Unexpected intent.");
       return;
   }

   boolean present = ignoreBatteryPresentState() ?
           true : intent.getBooleanExtra(BatteryManager.EXTRA_PRESENT, false);

   if (!present) {
       // No battery, return default values.
       gotBatteryStatus(true, 0, Double.POSITIVE_INFINITY, 1);
       return;
   }

   int current = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
   int max = intent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);
   double level = (double)current / (double)max;
   if (level < 0 || level > 1) {
       // Sanity check, assume default value in this case.
       level = 1.0;
   }

   int status = intent.getIntExtra(BatteryManager.EXTRA_STATUS, -1);
   boolean charging = !(status == BatteryManager.BATTERY_STATUS_DISCHARGING);

   // TODO(timvolodine) : add proper projection for chargingTime, dischargingTime.
   double chargingTime = (status == BatteryManager.BATTERY_STATUS_FULL) ?
           0 : Double.POSITIVE_INFINITY;
   double dischargingTime = Double.POSITIVE_INFINITY;

   gotBatteryStatus(charging, chargingTime, dischargingTime, level);
#endif
}

/**
 * Returns whether the BatteryStatusManager should ignore the battery present state.
 * It is required for some devices that incorrectly set the EXTRA_PRESENT property.
 */
Boolean BatteryStatusManager::IgnoreBatteryPresentState()
{
    // BatteryManager.EXTRA_PRESENT appears to be unreliable on Galaxy Nexus,
    // Android 4.2.1, it always reports false. See crbug.com/384348.
    return Build::MODEL.Equals("Galaxy Nexus");
}

void BatteryStatusManager::GotBatteryStatus(
  /* [in] */ Boolean charging,
  /* [in] */ Double chargingTime,
  /* [in] */ Double dischargingTime,
  /* [in] */ Double level)
{
  assert(0);
    // synchronized(mNativePtrLock) {
    //     if (mNativePtr != 0) {
    //         nativeGotBatteryStatus(mNativePtr, charging, chargingTime, dischargingTime, level);
    //     }
    // }
}

/**
 * Native JNI call
 * see content/browser/battery_status/battery_status_manager.cc
 */
void BatteryStatusManager::NativeGotBatteryStatus(
  /* [in] */ Int64 nativeBatteryStatusManager,
  /* [in] */ Boolean charging,
  /* [in] */ Double chargingTime,
  /* [in] */ Double dischargingTime,
  /* [in] */ Double level)
{
    Elastos_BatteryStatusManager_nativeGotBatteryStatus(
            THIS_PROBE(IInterface), (Handle32)nativeBatteryStatusManager, charging, chargingTime, dischargingTime, level);
}

AutoPtr<IInterface> BatteryStatusManager::GetInstance(
    /* [in] */ IInterface* appContext)
{
    AutoPtr<IContext> c = IContext::Probe(appContext);
    return TO_IINTERFACE(GetInstance(c));
}

Boolean BatteryStatusManager::Start(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 nativePtr)
{
    AutoPtr<BatteryStatusManager> mObj = (BatteryStatusManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BatteryStatusManager::Start, mObj is NULL");
        return FALSE;
    }
    return mObj->Start(nativePtr);
}

void BatteryStatusManager::Stop(
    /* [in] */ IInterface* obj)
{
    AutoPtr<BatteryStatusManager> mObj = (BatteryStatusManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BatteryStatusManager::Stop, mObj is NULL");
        return;
    }
    mObj->Stop();
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
