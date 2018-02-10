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

// wuweizuo automatic build .cpp file from .java file.

#include "elastos/droid/webkit/webview/chromium/native/content/browser/InterstitialPageDelegateElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/InterstitialPageDelegateElastos_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                   InterstitialPageDelegateElastos
//=====================================================================
InterstitialPageDelegateElastos::InterstitialPageDelegateElastos(
    /* [in] */ const String& htmlContent)
{
    mNativePtr = NativeInit(htmlContent);
}

HANDLE InterstitialPageDelegateElastos::GetNative()
{
    return mNativePtr;
}

ECode InterstitialPageDelegateElastos::OnProceed()
{
    return NOERROR;
}

ECode InterstitialPageDelegateElastos::OnDontProceed()
{
    return NOERROR;
}

ECode InterstitialPageDelegateElastos::CommandReceived(
    /* [in] */ const String& command)
{
    return NOERROR;
}

ECode InterstitialPageDelegateElastos::Proceed()
{
    if (mNativePtr != 0) {
        NativeProceed(mNativePtr);
    }

    return NOERROR;
}

ECode InterstitialPageDelegateElastos::DontProceed()
{
    if (mNativePtr != 0) {
        NativeDontProceed(mNativePtr);
    }

    return NOERROR;
}

ECode InterstitialPageDelegateElastos::OnNativeDestroyed()
{
    mNativePtr = 0;
    return NOERROR;
}

HANDLE InterstitialPageDelegateElastos::NativeInit(
    /* [in] */ const String& htmlContent)
{
    return Elastos_InterstitialPageDelegateAndroid_nativeInit(TO_IINTERFACE(this), htmlContent);
}

ECode InterstitialPageDelegateElastos::NativeProceed(
    /* [in] */ HANDLE nativeInterstitialPageDelegateElastos)
{
    Elastos_InterstitialPageDelegateAndroid_nativeProceed(TO_IINTERFACE(this),
            nativeInterstitialPageDelegateElastos);
    return NOERROR;
}

ECode InterstitialPageDelegateElastos::NativeDontProceed(
    /* [in] */ HANDLE nativeInterstitialPageDelegateElastos)
{
    Elastos_InterstitialPageDelegateAndroid_nativeDontProceed(TO_IINTERFACE(this),
            nativeInterstitialPageDelegateElastos);
    return NOERROR;
}

void InterstitialPageDelegateElastos::OnProceed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<InterstitialPageDelegateElastos> mObj = (InterstitialPageDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("InterstitialPageDelegateElastos", "InterstitialPageDelegateElastos::OnProceed, mObj is NULL");
        return;
    }
    mObj->OnProceed();
}

void InterstitialPageDelegateElastos::OnDontProceed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<InterstitialPageDelegateElastos> mObj = (InterstitialPageDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("InterstitialPageDelegateElastos", "InterstitialPageDelegateElastos::OnDontProceed, mObj is NULL");
        return;
    }
    mObj->OnDontProceed();
}

void InterstitialPageDelegateElastos::CommandReceived(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& command)
{
    AutoPtr<InterstitialPageDelegateElastos> mObj = (InterstitialPageDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("InterstitialPageDelegateElastos", "InterstitialPageDelegateElastos::CommandReceived, mObj is NULL");
        return;
    }
    mObj->CommandReceived(command);
}

void InterstitialPageDelegateElastos::OnNativeDestroyed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<InterstitialPageDelegateElastos> mObj = (InterstitialPageDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("InterstitialPageDelegateElastos", "InterstitialPageDelegateElastos::OnNativeDestroyed, mObj is NULL");
        return;
    }
    mObj->OnNativeDestroyed();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
