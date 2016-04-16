#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/BatteryStatusManager.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/BatteryStatusManager_dec.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
  mReceiver = new InnerBroadcastReceiver(this);
  CIntentFilter::New(IIntent::ACTION_BATTERY_CHANGED, (IIntentFilter**)&mFilter);
  context->GetApplicationContext((IContext**)&mAppContext);
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
  /* [in] */ Handle64 nativePtr)
{
    synchronized(mNativePtrLock) {
        AutoPtr<IIntent> stickyIntent;
        if (!mEnabled && (mAppContext->RegisterReceiver(mReceiver, mFilter, (IIntent**)&stickyIntent), stickyIntent) != NULL) {
            // success
            mNativePtr = nativePtr;
            mEnabled = TRUE;
        }
    }
    return mEnabled;
}

/**
 * Stop listening to intents.
 */
//@CalledByNative
void BatteryStatusManager::Stop()
{
    synchronized(mNativePtrLock) {
        if (mEnabled) {
            mAppContext->UnregisterReceiver(mReceiver);
            mNativePtr = 0;
            mEnabled = FALSE;
        }
    }
}

//@VisibleForTesting
void BatteryStatusManager::OnReceive(
  /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (!action.Equals(IIntent::ACTION_BATTERY_CHANGED)) {
        Logger::E(TAG, "Unexpected intent.");
        return;
    }

    Boolean present;//= IgnoreBatteryPresentState() ? TRUE : intent->GetBooleanExtra(IBatteryManager::EXTRA_PRESENT, FALSE);
    if (IgnoreBatteryPresentState()) {
        present = TRUE;
    } else {
        intent->GetBooleanExtra(IBatteryManager::EXTRA_PRESENT, FALSE, &present);
    }

    if (!present) {
        // No battery, return default values.
        GotBatteryStatus(TRUE, 0, Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY, 1);
        return;
    }

    Int32 current;
    intent->GetInt32Extra(IBatteryManager::EXTRA_LEVEL, -1, &current);
    Int32 max;
    intent->GetInt32Extra(IBatteryManager::EXTRA_SCALE, -1, &max);
    Double level = (Double)current / (Double)max;
    if (level < 0 || level > 1) {
        // Sanity check, assume default value in this case.
        level = 1.0;
    }

    Int32 status;
    intent->GetInt32Extra(IBatteryManager::EXTRA_STATUS, -1, &status);
    Boolean charging = !(status == IBatteryManager::BATTERY_STATUS_DISCHARGING);

    // TODO(timvolodine) : add proper projection for chargingTime, dischargingTime.
    Double chargingTime = (status == IBatteryManager::BATTERY_STATUS_FULL) ?
        0 : Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY;
    Double dischargingTime = Elastos::Core::Math::DOUBLE_POSITIVE_INFINITY;

    GotBatteryStatus(charging, chargingTime, dischargingTime, level);
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
    synchronized(mNativePtrLock) {
        if (mNativePtr != 0) {
            NativeGotBatteryStatus(mNativePtr, charging, chargingTime, dischargingTime, level);
        }
    }
}

/**
 * Native JNI call
 * see content/browser/battery_status/battery_status_manager.cc
 */
void BatteryStatusManager::NativeGotBatteryStatus(
  /* [in] */ Handle64 nativeBatteryStatusManager,
  /* [in] */ Boolean charging,
  /* [in] */ Double chargingTime,
  /* [in] */ Double dischargingTime,
  /* [in] */ Double level)
{
    Elastos_BatteryStatusManager_nativeGotBatteryStatus(
            TO_IINTERFACE(this), nativeBatteryStatusManager, charging, chargingTime, dischargingTime, level);
}

AutoPtr<IInterface> BatteryStatusManager::GetInstance(
    /* [in] */ IInterface* appContext)
{
    AutoPtr<IContext> c = IContext::Probe(appContext);
    return TO_IINTERFACE(GetInstance(c));
}

Boolean BatteryStatusManager::Start(
    /* [in] */ IInterface* obj,
    /* [in] */ Handle64 nativePtr)
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
