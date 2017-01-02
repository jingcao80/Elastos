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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/TouchDevice.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/api/TouchDevice_dec.h"

using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

//=====================================================================
//                             TouchDevice
//=====================================================================
TouchDevice::TouchDevice()
{
}

Int32 TouchDevice::MaxTouchPoints(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // // Android only tells us if the device belongs to a "Touchscreen Class"
    // // which only guarantees a minimum number of touch points. Be
    // // conservative and return the minimum, checking membership from the
    // // highest class down.
    // if (context.getPackageManager().hasSystemFeature(
    //         PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH_JAZZHAND)) {
    //     return 5;
    // } else if (context.getPackageManager().hasSystemFeature(
    //         PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH_DISTINCT)) {
    //     return 2;
    // } else if (context.getPackageManager().hasSystemFeature(
    //         PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH)) {
    //     return 2;
    // } else if (context.getPackageManager().hasSystemFeature(
    //         PackageManager.FEATURE_TOUCHSCREEN)) {
    //     return 1;
    // } else {
    //     return 0;
    // }

    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);

    Boolean hasSystemFeature = FALSE;
    packageManager->HasSystemFeature(IPackageManager::FEATURE_TOUCHSCREEN_MULTITOUCH_JAZZHAND, &hasSystemFeature);

    Boolean hasSystemFeature1 = FALSE;
    packageManager->HasSystemFeature(IPackageManager::FEATURE_TOUCHSCREEN_MULTITOUCH_DISTINCT, &hasSystemFeature1);

    Boolean hasSystemFeature2 = FALSE;
    packageManager->HasSystemFeature(IPackageManager::FEATURE_TOUCHSCREEN_MULTITOUCH, &hasSystemFeature2);

    Boolean hasSystemFeature3 = FALSE;
    packageManager->HasSystemFeature(IPackageManager::FEATURE_TOUCHSCREEN, &hasSystemFeature3);

    if (hasSystemFeature) {
        return 5;
    }
     else if (hasSystemFeature1) {
        return 2;
    }
     else if (hasSystemFeature2) {
        return 2;
    }
     else if (hasSystemFeature3) {
        return 1;
    }
    else {
        return 0;
    }
}

Int32 TouchDevice::MaxTouchPoints(
    /* [in] */ IInterface* context)
{
    IContext* c = IContext::Probe(context);
    return MaxTouchPoints(c);
}


} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


