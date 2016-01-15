// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_POWERSAVEBLOCKER_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_POWERSAVEBLOCKER_H_

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Base {

class ViewElastos;

} // namespace Base
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

using Elastos::Droid::Webkit::Ui::Base::ViewElastos;

// package org.chromium.content.browser;
// import org.chromium.base.CalledByNative;
// import org.chromium.ui.base.ViewAndroid;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_POWERSAVEBLOCKER_H_
