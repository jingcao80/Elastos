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

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_VIEWANDROID_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_VIEWANDROID_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/ViewElastosDelegate.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/WindowElastos.h"

// package org.chromium.ui.base;
// import android.view.View;
// import org.chromium.base.JNINamespace;

using Elastos::Droid::View::IView;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::ViewElastosDelegate;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::WindowElastos;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

/**
  * From the Chromium architecture point of view, ViewElastos and its native counterpart
  * serve purpose of representing Android view where Chrome expects to have a cross platform
  * handle to the system view type. As Views are Java object on Android, this ViewElastos
  * and its native counterpart provide the expected abstractions on the C++ side and allow
  * it to be flexibly glued to an actual Android Java View at runtime.
  *
  * It should only be used where access to Android Views is needed from the C++ code.
  */
// @JNINamespace("ui")
class ViewElastos
    : public Object
{
public:
    /**
      * Constructs a View object.
      */
    ViewElastos(
        /* [in] */ WindowElastos* nativeWindow,
        /* [in] */ ViewElastosDelegate* viewElastosDelegate);

    virtual CARAPI_(AutoPtr<ViewElastosDelegate>) GetViewElastosDelegate();

    /**
      * Destroys the c++ ViewElastos object if one has been created.
      */
    virtual CARAPI Destroy();

    /**
      * Returns a pointer to the c++ AndroidWindow object.
      * @return A pointer to the c++ AndroidWindow.
      */
    virtual CARAPI_(HANDLE) GetNativePointer();

    /**
      * Set KeepScreenOn flag. If the flag already set, increase mKeepScreenOnCount.
      */
    virtual CARAPI IncrementKeepScreenOnCount();

    /**
      * Decrease mKeepScreenOnCount, if it is decreased to 0, remove the flag.
      */
    virtual CARAPI DecrementKeepScreenOnCount();

private:
    CARAPI_(HANDLE) NativeInit(
        /* [in] */ HANDLE windowPtr);

    CARAPI NativeDestroy(
        /* [in] */ HANDLE nativeViewElastos);

private:
    // Native pointer to the c++ ViewElastos object.
    HANDLE mNativeViewElastos;
    AutoPtr<ViewElastosDelegate> mViewElastosDelegate;
    AutoPtr<WindowElastos> mWindowElastos;
    Int32 mKeepScreenOnCount;
    AutoPtr<IView> mKeepScreenOnView;
};

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_VIEWANDROID_H_

