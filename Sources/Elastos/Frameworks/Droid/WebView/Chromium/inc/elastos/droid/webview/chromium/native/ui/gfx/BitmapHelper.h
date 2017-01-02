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

// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_GFX_BITMAPHELPER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_GFX_BITMAPHELPER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui.gfx;
// import android.content.res.Resources;
// import android.graphics.Bitmap;
// import android.graphics.BitmapFactory;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Gfx {

/**
  * Helper class to decode and sample down bitmap resources.
  */
// @JNINamespace("gfx")
class BitmapHelper
    : public Object
{
public:
    static CARAPI_(void*) ElaBitmapHelperCallback_Init();

private:
    // @CalledByNative return IBitmap
    static CARAPI_(AutoPtr<IInterface>) CreateBitmap(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 bitmapFormatValue);

    /**
      * Decode and sample down a bitmap resource to the requested width and height.
      *
      * @param name The resource name of the bitmap to decode.
      * @param reqWidth The requested width of the resulting bitmap.
      * @param reqHeight The requested height of the resulting bitmap.
      * @return A bitmap sampled down from the original with the same aspect ratio and dimensions.
      *         that are equal to or greater than the requested width and height.
      */
    // @CalledByNative return IBitmap
    static CARAPI_(AutoPtr<IInterface>) DecodeDrawableResource(
        /* [in] */ const String& name,
        /* [in] */ Int32 reqWidth,
        /* [in] */ Int32 reqHeight);

    // http://developer.android.com/training/displaying-bitmaps/load-bitmap.html
    static CARAPI_(Int32) CalculateInSampleSize(
        /* [in] */ IBitmapFactoryOptions* options,
        /* [in] */ Int32 reqWidth,
        /* [in] */ Int32 reqHeight);

    /**
      * Provides a matching integer constant for the Bitmap.Config value passed.
      *
      * @param bitmapConfig The Bitmap Configuration value.
      * @return Matching integer constant for the Bitmap.Config value passed.
      */
    // @CalledByNative
    static CARAPI_(Int32) GetBitmapFormatForConfig(
        /* [in] */ BitmapConfig bitmapConfig);

    static CARAPI_(Int32) GetBitmapFormatForConfig(
        /* [in] */ IInterface* bitmapConfig);

    /**
      * Provides a matching Bitmap.Config for the enum config value passed.
      *
      * @param bitmapFormatValue The Bitmap Configuration enum value.
      * @return Matching Bitmap.Config  for the enum value passed.
      */
    static CARAPI_(BitmapConfig) GetBitmapConfigForFormat(
        /* [in] */ Int32 bitmapFormatValue);
};

} // namespace Gfx
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_GFX_BITMAPHELPER_H_

