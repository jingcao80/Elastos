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
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ScreenOrientationProvider.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ScreenOrientationProvider_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApplicationStatus.h"
#include "elastos/droid/webkit/webview/chromium/native/content/common/ScreenOrientationValues.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Webkit::Webview::Chromium::Base::ApplicationStatus;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Common::ScreenOrientationValues;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                      ScreenOrientationProvider
//=====================================================================
const String ScreenOrientationProvider::TAG("ScreenOrientationProvider");

ScreenOrientationProvider::ScreenOrientationProvider()
{
}

AutoPtr<IInterface> ScreenOrientationProvider::Create()
{
    return TO_IINTERFACE(new ScreenOrientationProvider());
}

ECode ScreenOrientationProvider::LockOrientation(
    /* [in] */ Byte orientations)
{
    AutoPtr<IActivity> activity = ApplicationStatus::GetLastTrackedFocusedActivity();
    if (activity == NULL) {
        return NOERROR;
    }

    Int32 orientation = GetOrientationFromWebScreenOrientations(orientations);
    if (orientation == IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED) {
        return NOERROR;
    }

    return activity->SetRequestedOrientation(orientation);
}

ECode ScreenOrientationProvider::UnlockOrientation()
{
    AutoPtr<IActivity> activity = ApplicationStatus::GetLastTrackedFocusedActivity();
    if (activity == NULL) {
        return NOERROR;
    }

    return activity->SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED);
}

Int32 ScreenOrientationProvider::GetOrientationFromWebScreenOrientations(
    /* [in] */ Byte orientations)
{
    switch (orientations) {
        case ScreenOrientationValues::DEFAULT:
            return IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED;
        case ScreenOrientationValues::PORTRAIT_PRIMARY:
            return IActivityInfo::SCREEN_ORIENTATION_PORTRAIT;
        case ScreenOrientationValues::PORTRAIT_SECONDARY:
            return IActivityInfo::SCREEN_ORIENTATION_REVERSE_PORTRAIT;
        case ScreenOrientationValues::LANDSCAPE_PRIMARY:
            return IActivityInfo::SCREEN_ORIENTATION_LANDSCAPE;
        case ScreenOrientationValues::LANDSCAPE_SECONDARY:
            return IActivityInfo::SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
        case ScreenOrientationValues::PORTRAIT:
            return IActivityInfo::SCREEN_ORIENTATION_SENSOR_PORTRAIT;
        case ScreenOrientationValues::LANDSCAPE:
            return IActivityInfo::SCREEN_ORIENTATION_SENSOR_LANDSCAPE;
        case ScreenOrientationValues::ANY:
            return IActivityInfo::SCREEN_ORIENTATION_FULL_SENSOR;
        default:
            Slogger::W(TAG, "Trying to lock to unsupported orientation!");
            return IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED;
    }

    return 0;
}

void ScreenOrientationProvider::LockOrientation(
    /* [in] */ IInterface* obj,
    /* [in] */ Byte orientations)
{
    AutoPtr<ScreenOrientationProvider> mObj = (ScreenOrientationProvider*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "ScreenOrientationProvider::LockOrientation, mObj is NULL");
        return;
    }
    mObj->LockOrientation(orientations);
}

void ScreenOrientationProvider::UnlockOrientation(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ScreenOrientationProvider> mObj = (ScreenOrientationProvider*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "ScreenOrientationProvider::UnlockOrientation, mObj is NULL");
        return;
    }
    mObj->UnlockOrientation();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
