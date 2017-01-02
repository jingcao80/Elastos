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

#ifndef __ELASTOS_DROID_APP_UIAUTOMATION_CONNECTION_H__
#define __ELASTOS_DROID_APP_UIAUTOMATION_CONNECTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::AccessibilityService::IIAccessibilityServiceClient;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IWindowAnimationFrameStats;
using Elastos::Droid::View::IWindowContentFrameStats;
using Elastos::Droid::View::Accessibility::IIAccessibilityManager;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * This is a remote object that is passed from the shell to an instrumentation
 * for enabling access to privileged operations which the shell can do and the
 * instrumentation cannot. These privileged operations are needed for implementing
 * a {@link UiAutomation} that enables across application testing by simulating
 * user actions and performing screen introspection.
 *
 * @hide
 */
class UiAutomationConnection
    : public Object
    , public IIUiAutomationConnection
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    UiAutomationConnection();

    virtual ~UiAutomationConnection();

    CARAPI constructor();

    CARAPI Connect(
        /* [in] */ IIAccessibilityServiceClient* client);

    CARAPI Disconnect();

    CARAPI InjectInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Boolean sync,
        /* [out] */ Boolean* result);

    CARAPI SetRotation(
        /* [in] */ Int32 rotation,
        /* [out] */ Boolean* result);

    CARAPI TakeScreenshot(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IBitmap** bmp);

    CARAPI ClearWindowContentFrameStats(
        /* [in] */ Int32 windowId,
        /* [out] */ Boolean* result);

    CARAPI GetWindowContentFrameStats(
        /* [in] */ Int32 windowId,
        /* [out] */ IWindowContentFrameStats** stats);

    CARAPI ClearWindowAnimationFrameStats();

    CARAPI GetWindowAnimationFrameStats(
        /* [out] */ IWindowAnimationFrameStats** stats);

    CARAPI ExecuteShellCommand(
        /* [in] */ const String& command,
        /* [in] */ IParcelFileDescriptor* sink);

    CARAPI Shutdown();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI RegisterUiTestAutomationServiceLocked(
        /* [in] */ IIAccessibilityServiceClient* client);

    CARAPI UnregisterUiTestAutomationServiceLocked();

    CARAPI StoreRotationStateLocked();

    CARAPI RestoreRotationStateLocked();

    Boolean IsConnectedLocked();

    CARAPI ThrowIfShutdownLocked();

    CARAPI ThrowIfNotConnectedLocked();

    CARAPI ThrowIfCalledByNotTrustedUidLocked();

private:
    static const String TAG;
    static const Int32 INITIAL_FROZEN_ROTATION_UNSPECIFIED;

    AutoPtr<IIWindowManager> mWindowManager;
    AutoPtr<IIAccessibilityManager> mAccessibilityManager;

    Object mLock;

    AutoPtr<IBinder> mToken;

    Int32 mInitialFrozenRotation;

    AutoPtr<IIAccessibilityServiceClient> mClient;

    Boolean mIsShutdown;

    Int32 mOwningUid;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_UIAUTOMATION_CONNECTION_H__
