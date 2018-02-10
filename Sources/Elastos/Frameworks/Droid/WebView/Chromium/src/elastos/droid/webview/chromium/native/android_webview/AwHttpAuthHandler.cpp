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

#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwHttpAuthHandler.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwHttpAuthHandler_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

AwHttpAuthHandler::AwHttpAuthHandler(
    /* [in] */ HANDLE nativeAwHttpAuthHandler,
    /* [in] */ Boolean firstAttempt)
    : mNativeAwHttpAuthHandler(nativeAwHttpAuthHandler)
    , mFirstAttempt(firstAttempt)
{
}

void AwHttpAuthHandler::Proceed(
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    if (mNativeAwHttpAuthHandler != 0) {
        NativeProceed(mNativeAwHttpAuthHandler, username, password);
        mNativeAwHttpAuthHandler = 0;
    }
}

void AwHttpAuthHandler::Cancel()
{
    if (mNativeAwHttpAuthHandler != 0) {
        NativeCancel(mNativeAwHttpAuthHandler);
        mNativeAwHttpAuthHandler = 0;
    }
}

Boolean AwHttpAuthHandler::IsFirstAttempt()
{
     return mFirstAttempt;
}

//@CalledByNative return AwHttpAuthHandler
AutoPtr<IInterface> AwHttpAuthHandler::Create(
    /* [in] */ HANDLE nativeAwAuthHandler,
    /* [in] */ Boolean firstAttempt)
{
    AutoPtr<AwHttpAuthHandler> handler = new AwHttpAuthHandler(nativeAwAuthHandler, firstAttempt);
    AutoPtr<IInterface> result = TO_IINTERFACE(handler);
    return result;
}

//@CalledByNative
void AwHttpAuthHandler::HandlerDestroyed()
{
    mNativeAwHttpAuthHandler = 0;
}

void AwHttpAuthHandler::NativeProceed(
    /* [in] */ HANDLE nativeAwHttpAuthHandler,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    Elastos_AwHttpAuthHandler_nativeProceed(TO_IINTERFACE(this), nativeAwHttpAuthHandler, username, password);
}

void AwHttpAuthHandler::NativeCancel(
    /* [in] */ HANDLE nativeAwHttpAuthHandler)
{
    Elastos_AwHttpAuthHandler_nativeCancel(TO_IINTERFACE(this), nativeAwHttpAuthHandler);
}
//callback function definition
void AwHttpAuthHandler::HandlerDestroyed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwHttpAuthHandler> mObj = (AwHttpAuthHandler*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwHttpAuthHandler", "AwHttpAuthHandler::HandlerDestroyed, mObj is NULL");
        return;
    }
    mObj->HandlerDestroyed();
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
