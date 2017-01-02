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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_SCREENTYPE_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_SCREENTYPE_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

class ScreenType
{
public:
    static CARAPI IsPhone(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    static CARAPI IsHybrid(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    static CARAPI IsTablet(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(Int32) GetScreenType(
        /* [in] */ IContext* context);

private:
    // Device type reference
    static Int32 sDeviceType;

    // Device types
    const static Int32 DEVICE_PHONE;
    const static Int32 DEVICE_HYBRID;
    const static Int32 DEVICE_TABLET;
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_SCREENTYPE_H__
