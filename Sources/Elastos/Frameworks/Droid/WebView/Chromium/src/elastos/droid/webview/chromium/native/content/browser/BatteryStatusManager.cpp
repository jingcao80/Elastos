//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/BatteryStatusManager.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/BatteryStatusManager_dec.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
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
  /* [in] */ HANDLE nativePtr)
{
    {    AutoLock syncLock(mNativePtrLock);
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
    {    AutoLock syncLock(mNativePtrLock);
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
    {    AutoLock syncLock(mNativePtrLock);
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
  /* [in] */ HANDLE nativeBatteryStatusManager,
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
    /* [in] */ HANDLE nativePtr)
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
