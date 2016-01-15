// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_UI_BASE_TOUCHDEVICE_H_
#define _ELASTOS_DROID_WEBKIT_UI_BASE_TOUCHDEVICE_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui.base;
// import android.content.Context;
// import android.content.pm.PackageManager;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Base {

/**
  * Simple proxy to let us query the touch device from C++
  */
// @JNINamespace("ui")
class TouchDevice
    : public Object
{
public:
    static CARAPI_(void*) ElaTouchDeviceCallback_Init();

private:
    /**
      * Static methods only so make constructor private.
      */
    TouchDevice();

    /**
      * Returns the number of supported touch points.
      *
      * @return Maximum supported touch points.
      */
    // @CalledByNative
    static CARAPI_(Int32) MaxTouchPoints(
        /* [in] */ IContext* context);

    static CARAPI_(Int32) MaxTouchPoints(
        /* [in] */ IInterface* context);

};

} // namespace Base
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_UI_BASE_TOUCHDEVICE_H_

