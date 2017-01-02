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

