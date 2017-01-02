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

#include "elastos/droid/webkit/webview/chromium/native/android_webview/JsResultHandler.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClientBridge.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {


#ifndef CAR_INTERFACE_IMPL_JSRESULTHANDLER
#define CAR_INTERFACE_IMPL_JSRESULTHANDLER(ClassName, SupperClassName)       \
                                                           \
    UInt32 ClassName::AddRef()                             \
    {                                                      \
        return SupperClassName::AddRef();                        \
    }                                                      \
                                                           \
    UInt32 ClassName::Release()                            \
    {                                                      \
        return SupperClassName::Release();                       \
    }                                                      \
                                                           \
    PInterface ClassName::Probe(                           \
        /* [in] */ REIID riid)                             \
    {                                                      \
        return SupperClassName::Probe(riid);               \
    }                                                      \
                                                           \
    ECode ClassName::GetInterfaceID(                       \
        /* [in] */ IInterface* object,                     \
        /* [out] */ InterfaceID* iid)                      \
    {                                                      \
        VALIDATE_NOT_NULL(iid);                            \
                                                           \
        return SupperClassName::GetInterfaceID(object, iid); \
    }
#endif
CAR_INTERFACE_IMPL_JSRESULTHANDLER(JsResultHandler, Object);
//===============================================================
//               JsResultHandler::ConfirmRunnable
//===============================================================
CAR_INTERFACE_IMPL(JsResultHandler::ConfirmRunnable, Object, IRunnable);

JsResultHandler::ConfirmRunnable::ConfirmRunnable(
    /* [in] */ JsResultHandler* owner,
    /* [in] */ const String& promptResult)
    : mOwner(owner)
    , mPromptResult(promptResult)
{
}

ECode JsResultHandler::ConfirmRunnable::Run()
{
    if (mOwner->mBridge != NULL)
        mOwner->mBridge->ConfirmJsResult(mOwner->mId, mPromptResult);
    mOwner->mBridge = NULL;

    return NOERROR;
}

//===============================================================
//               JsResultHandler::CancelRunnable
//===============================================================
CAR_INTERFACE_IMPL(JsResultHandler::CancelRunnable, Object, IRunnable);

JsResultHandler::CancelRunnable::CancelRunnable(
    /* [in] */ JsResultHandler* owner)
    : mOwner(owner)
{
}

ECode JsResultHandler::CancelRunnable::Run()
{
    if (mOwner->mBridge != NULL)
        mOwner->mBridge->CancelJsResult(mOwner->mId);
    mOwner->mBridge = NULL;

    return NOERROR;
}

//===============================================================
//                      JsResultHandler
//===============================================================

JsResultHandler::JsResultHandler(
    /* [in] */ AwContentsClientBridge* bridge,
    /* [in] */ Int32 id)
    : mBridge(bridge)
    , mId(id)
{
}

//@Override
ECode JsResultHandler::Confirm()
{
    return Confirm(String(NULL));
}

//@Override
ECode JsResultHandler::Confirm(
    /* [in] */ const String& promptResult)
{
    AutoPtr<IRunnable> runnable =  new ConfirmRunnable(this, promptResult);
    ThreadUtils::RunOnUiThread(runnable);
    return NOERROR;
}

//@Override
ECode JsResultHandler::Cancel()
{
    AutoPtr<IRunnable> runnable =  new CancelRunnable(this);
    ThreadUtils::RunOnUiThread(runnable);
    return NOERROR;
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
