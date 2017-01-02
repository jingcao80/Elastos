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

#include "elastos/droid/webkit/webview/chromium/native/ui/base/ViewElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/api/ViewElastos_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

//=====================================================================
//                             ViewElastos
//=====================================================================
ViewElastos::ViewElastos(
    /* [in] */ WindowElastos* nativeWindow,
    /* [in] */ ViewElastosDelegate* viewElastosDelegate)
    : mNativeViewElastos(0)
    , mViewElastosDelegate(viewElastosDelegate)
    , mWindowElastos(nativeWindow)
    , mKeepScreenOnCount(0)
    , mKeepScreenOnView(NULL)
{
    // ==================before translated======================
    // mWindowElastos = nativeWindow;
    // mViewElastosDelegate = viewAndroidDelegate;
    // mNativeViewElastos = nativeInit(mWindowElastos.getNativePointer());

    mNativeViewElastos = NativeInit(mWindowElastos->GetNativePointer());
}

AutoPtr<ViewElastosDelegate> ViewElastos::GetViewElastosDelegate()
{
    // ==================before translated======================
    // return mViewElastosDelegate;

    return mViewElastosDelegate;
}

ECode ViewElastos::Destroy()
{
    // ==================before translated======================
    // if (mNativeViewElastos != 0) {
    //     nativeDestroy(mNativeViewElastos);
    //     mNativeViewElastos = 0;
    // }

    if (mNativeViewElastos != 0) {
        NativeDestroy(mNativeViewElastos);
        mNativeViewElastos = 0;
    }
    return NOERROR;
}

Handle64 ViewElastos::GetNativePointer()
{
    // ==================before translated======================
    // return mNativeViewElastos;

    return mNativeViewElastos;
}

ECode ViewElastos::IncrementKeepScreenOnCount()
{
    // ==================before translated======================
    // mKeepScreenOnCount++;
    // if (mKeepScreenOnCount == 1) {
    //     mKeepScreenOnView = mViewElastosDelegate.acquireAnchorView();
    //     mViewElastosDelegate.setAnchorViewPosition(mKeepScreenOnView, 0, 0, 0, 0);
    //     mKeepScreenOnView.setKeepScreenOn(true);
    // }

    mKeepScreenOnCount++;
    if (mKeepScreenOnCount == 1) {
        mKeepScreenOnView = mViewElastosDelegate->AcquireAnchorView();
        mViewElastosDelegate->SetAnchorViewPosition(mKeepScreenOnView, 0, 0, 0, 0);
        mKeepScreenOnView->SetKeepScreenOn(TRUE);
    }
    return NOERROR;
}

ECode ViewElastos::DecrementKeepScreenOnCount()
{
    // ==================before translated======================
    // assert mKeepScreenOnCount > 0;
    // mKeepScreenOnCount--;
    // if (mKeepScreenOnCount == 0) {
    //     mViewElastosDelegate.releaseAnchorView(mKeepScreenOnView);
    //     mKeepScreenOnView = null;
    // }

    assert (mKeepScreenOnCount > 0);
    --mKeepScreenOnCount;
    if (mKeepScreenOnCount == 0) {
        mViewElastosDelegate->ReleaseAnchorView(mKeepScreenOnView);
        mKeepScreenOnView = NULL;
    }
    return NOERROR;
}

Handle64 ViewElastos::NativeInit(
    /* [in] */ Handle64 windowPtr)
{
    return Elastos_ViewAndroid_nativeInit(TO_IINTERFACE(this), windowPtr);
}

ECode ViewElastos::NativeDestroy(
    /* [in] */ Handle64 nativeViewElastos)
{
    Elastos_ViewAndroid_nativeDestroy(TO_IINTERFACE(this), (Handle64)nativeViewElastos);
    return NOERROR;
}

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


