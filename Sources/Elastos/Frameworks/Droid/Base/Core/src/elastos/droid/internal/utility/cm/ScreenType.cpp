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

#include "elastos/droid/internal/utility/cm/ScreenType.h"
#include "elastos/droid/view/CDisplayInfo.h"
#include <elastos/core/Math.h>

using Elastos::Droid::View::CDisplayInfo;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

// Device type reference
Int32 ScreenType::sDeviceType = -1;

// Device types
const Int32 ScreenType::DEVICE_PHONE = 0;
const Int32 ScreenType::DEVICE_HYBRID = 1;
const Int32 ScreenType::DEVICE_TABLET = 2;

ECode ScreenType::IsPhone(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetScreenType(context) == DEVICE_PHONE;
    return NOERROR;
}

ECode ScreenType::IsHybrid(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetScreenType(context) == DEVICE_HYBRID;
    return NOERROR;
}

ECode ScreenType::IsTablet(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetScreenType(context) == DEVICE_TABLET;
    return NOERROR;
}

Int32 ScreenType::GetScreenType(
    /* [in] */ IContext* context)
{
    if (sDeviceType == -1) {
        AutoPtr<IInterface> service;
        context->GetSystemService(IContext::WINDOW_SERVICE, ((IInterface**)&service));
        AutoPtr<IWindowManager> wm = IWindowManager::Probe(service);
        AutoPtr<IDisplayInfo> outDisplayInfo;
        CDisplayInfo::New((IDisplayInfo**)&outDisplayInfo);
        AutoPtr<IDisplay> display;
        wm->GetDefaultDisplay((IDisplay**)&display);
        Boolean b;
        display->GetDisplayInfo(outDisplayInfo, &b);
        Int32 height, width, densityDpi;
        outDisplayInfo->GetLogicalHeight(&height);
        outDisplayInfo->GetLogicalWidth(&width);
        outDisplayInfo->GetLogicalDensityDpi(&densityDpi);
        Int32 shortSize = Elastos::Core::Math::Min(height, width);
        Int32 shortSizeDp = shortSize * IDisplayMetrics::DENSITY_DEFAULT / densityDpi;

        if (shortSizeDp < 600) {
            // 0-599dp: "phone" UI with a separate status & navigation bar
            sDeviceType =  DEVICE_PHONE;
        }
        else if (shortSizeDp < 720) {
            // 600-719dp: "phone" UI with modifications for larger screens
            sDeviceType = DEVICE_HYBRID;
        }
        else {
            // 720dp: "tablet" UI with a single combined status & navigation bar
            sDeviceType = DEVICE_TABLET;
        }
    }
    return sDeviceType;
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
