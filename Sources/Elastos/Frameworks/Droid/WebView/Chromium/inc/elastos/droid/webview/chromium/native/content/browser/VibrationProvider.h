// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_VIBRATIONPROVIDER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_VIBRATIONPROVIDER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.content.browser;
// import android.content.Context;
// import android.content.pm.PackageManager;
// import android.media.AudioManager;
// import android.os.Vibrator;
// import android.util.Log;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::IVibrator;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * This is the implementation of the C++ counterpart VibrationProvider.
  */
// @JNINamespace("content")
class VibrationProvider
    : public Object
{
public:
    static CARAPI_(void*) ElaVibrationProviderCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ IInterface* context);

    static CARAPI_(void) Vibrate(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 milliseconds);

    static CARAPI_(void) CancelVibration(
        /* [in] */ IInterface* obj);

    VibrationProvider(
        /* [in] */ IContext* context);

    // @CalledByNative
    static CARAPI_(AutoPtr<VibrationProvider>) Create(
        /* [in] */ IContext* context);

    // @CalledByNative
    CARAPI Vibrate(
        /* [in] */ Int64 milliseconds);

    // @CalledByNative
    CARAPI CancelVibration();

private:
    static const String TAG;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IVibrator> mVibrator;
    Boolean mHasVibratePermission;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_VIBRATIONPROVIDER_H_
