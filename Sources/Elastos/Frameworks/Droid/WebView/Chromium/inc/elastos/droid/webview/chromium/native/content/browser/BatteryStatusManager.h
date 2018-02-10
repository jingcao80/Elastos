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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_BATTERYSTATUSMAMAGER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_BATTERYSTATUSMAMAGER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::Build;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
    : public BroadcastReceiver
  {
  public:
    TO_STRING_IMPL("BatteryStatusManager::InnerBroadcastReceiver")

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
      /* [in] */ HANDLE nativePtr);

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
        /* [in] */ HANDLE nativePtr);

    static CARAPI_(void) Stop(
        /* [in] */ IInterface* obj);

    /**
     * Native JNI call
     * see content/browser/battery_status/battery_status_manager.cc
     */
    CARAPI_(void) NativeGotBatteryStatus(
      /* [in] */ HANDLE nativeBatteryStatusManager,
      /* [in] */ Boolean charging,
      /* [in] */ Double chargingTime,
      /* [in] */ Double dischargingTime,
      /* [in] */ Double level);

private:
    static const String TAG;

    // A reference to the application context in order to acquire the SensorService.
    AutoPtr<IContext> mAppContext;
    AutoPtr<IIntentFilter> mFilter;
    AutoPtr<IBroadcastReceiver> mReceiver;

    // Non-zero if and only if we're listening for events.
    // To avoid race conditions on the C++ side, access must be synchronized.
    HANDLE mNativePtr;

    // The lock to access the mNativePtr.
    Object mNativePtrLock;

    Boolean mEnabled;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_BATTERYSTATUSMAMAGER_H__
