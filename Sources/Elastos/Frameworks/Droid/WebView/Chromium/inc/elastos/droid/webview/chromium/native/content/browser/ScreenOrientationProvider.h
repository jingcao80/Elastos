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

// wuweizuo automatic build .h file from .java file.
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SCREENORIENTATIONPROVIDER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SCREENORIENTATIONPROVIDER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.content.browser;
// import android.app.Activity;
// import android.content.pm.ActivityInfo;
// import android.util.Log;
// import com.google.common.annotations.VisibleForTesting;
// import org.chromium.base.ApplicationStatus;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.content.common.ScreenOrientationValues;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::Pm::IActivityInfo;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * This is the implementation of the C++ counterpart ScreenOrientationProvider.
  */
// @JNINamespace("content")
class ScreenOrientationProvider
    : public Object
{
public:
    // @VisibleForTesting
    // @CalledByNative return ScreenOrientationProvider
    static CARAPI_(AutoPtr<IInterface>) Create();

    // @CalledByNative
    virtual CARAPI LockOrientation(
        /* [in] */ Byte orientations);

    // @CalledByNative
    virtual CARAPI UnlockOrientation();

    static CARAPI_(void*) ElaScreenOrientationProviderCallback_Init();

private:

    static CARAPI_(void) LockOrientation(
        /* [in] */ IInterface* obj,
        /* [in] */ Byte orientations);

    static CARAPI_(void) UnlockOrientation(
        /* [in] */ IInterface* obj);

    ScreenOrientationProvider();

    CARAPI_(Int32) GetOrientationFromWebScreenOrientations(
        /* [in] */ Byte orientations);

private:
    static const String TAG;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SCREENORIENTATIONPROVIDER_H_
