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

#include "elastos/droid/javaproxy/CIUiAutomationConnectionNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IIUiAutomationConnection;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIUiAutomationConnectionNative::TAG("CIUiAutomationConnectionNative");

CAR_INTERFACE_IMPL_2(CIUiAutomationConnectionNative, Object, IIUiAutomationConnection, IBinder)

CAR_OBJECT_IMPL(CIUiAutomationConnectionNative)

CIUiAutomationConnectionNative::~CIUiAutomationConnectionNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIUiAutomationConnectionNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::Connect(
    /* [in] */ IIAccessibilityServiceClient* client)
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::Disconnect()
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::InjectInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Boolean sync,
    /* [out] */ Boolean* result)
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::SetRotation(
    /* [in] */ Int32 rotation,
    /* [out] */ Boolean* result)
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::TakeScreenshot(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** bitmap)
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::Shutdown()
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::ClearWindowContentFrameStats(
    /* [in] */ Int32 windowId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::GetWindowContentFrameStats(
    /* [in] */ Int32 windowId,
    /* [out] */ IWindowContentFrameStats** stats)
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::ClearWindowAnimationFrameStats()
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::GetWindowAnimationFrameStats(
    /* [out] */ IWindowAnimationFrameStats** stats)
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::ExecuteShellCommand(
    /* [in] */ const String& command,
    /* [in] */ IParcelFileDescriptor* fd)
{
    assert(0);
    return NOERROR;
}

ECode CIUiAutomationConnectionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIUiAutomationConnectionNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIUiAutomationConnectionNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
