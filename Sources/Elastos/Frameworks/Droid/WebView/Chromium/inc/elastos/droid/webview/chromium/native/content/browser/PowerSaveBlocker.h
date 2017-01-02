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
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_POWERSAVEBLOCKER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_POWERSAVEBLOCKER_H_

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

class ViewElastos;

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::ViewElastos;

// package org.chromium.content.browser;
// import org.chromium.base.CalledByNative;
// import org.chromium.ui.base.ViewAndroid;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class PowerSaveBlocker
{
public:
    static CARAPI_(void*) ElaPowerSaveBlockerCallback_Init();

private:
    static CARAPI_(void) ApplyBlock(
        /* [in] */ IInterface* view);

    static CARAPI_(void) RemoveBlock(
        /* [in] */ IInterface* view);

    // @CalledByNative
    static CARAPI ApplyBlock(
        /* [in] */ ViewElastos* view);

    // @CalledByNative
    static CARAPI RemoveBlock(
        /* [in] */ ViewElastos* view);
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_POWERSAVEBLOCKER_H_
