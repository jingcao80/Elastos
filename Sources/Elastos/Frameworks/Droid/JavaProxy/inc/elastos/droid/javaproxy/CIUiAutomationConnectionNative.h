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

#ifndef __ELASTOS_DROID_JAVAPROXY_CIUIAUTOMATIONCONNECTIONNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIUIAUTOMATIONCONNECTIONNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIUiAutomationConnectionNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::AccessibilityService::IIAccessibilityServiceClient;
using Elastos::Droid::App::IIUiAutomationConnection;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IWindowContentFrameStats;
using Elastos::Droid::View::IWindowAnimationFrameStats;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIUiAutomationConnectionNative)
    , public Object
    , public IIUiAutomationConnection
    , public IBinder
{
public:
    ~CIUiAutomationConnectionNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

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
        /* [out] */ IBitmap** bitmap);

    CARAPI Shutdown();

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
        /* [in] */ IParcelFileDescriptor* fd);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CIUIAUTOMATIONCONNECTIONNATIVE_H__
