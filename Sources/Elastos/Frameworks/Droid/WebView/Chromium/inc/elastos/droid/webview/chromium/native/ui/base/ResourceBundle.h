// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_RESOURCEBUNDLE_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_RESOURCEBUNDLE_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui.base;
// import android.content.Context;
// import android.content.res.AssetFileDescriptor;
// import android.content.res.AssetManager;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import java.io.IOException;

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

/**
  * This class provides the resource bundle related methods for the native library.
  */
// @JNINamespace("ui")
class ResourceBundle
    : public Object
{
public:
    // @CalledByNative
    static CARAPI_(Boolean) AssetContainedInApk(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& filename);

    static CARAPI_(void*) ElaResourceBundleCallback_Init();

private:
    static CARAPI_(Boolean) AssetContainedInApk(
        /* [in] */ IInterface* ctx,
        /* [in] */ const String& filename);

};

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_RESOURCEBUNDLE_H_

