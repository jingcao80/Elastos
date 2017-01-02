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

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_DEVICEFORMFACTOR_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_DEVICEFORMFACTOR_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui.base;
// import android.content.Context;
// import org.chromium.base.CalledByNative;

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

/**
  * UI utilities for accessing form factor information.
  */
class DeviceFormFactor
    : public Object
{
public:
    /**
      * @param context Android's context
      * @return        Whether the app is should treat the device as a tablet for layout.
      */
    // @CalledByNative
    static CARAPI_(Boolean) IsTablet(
        /* [in] */ IContext* context);

    static CARAPI_(void*) ElaDeviceFormFactorCallback_Init();

private:
    static CARAPI_(Boolean) IsTablet(
        /* [in] */ IInterface* context);

private:
    /**
      * The minimum width that would classify the device as a tablet.
      */
    static const Int32 MINIMUM_TABLET_WIDTH_DP = 600;
    static Boolean sIsTablet;
};

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_DEVICEFORMFACTOR_H_

