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

#ifndef __ELASTOS_DROID_CONTENT_RES_CCOMPATIBILITYINFOHELPER_H__
#define __ELASTOS_DROID_CONTENT_RES_CCOMPATIBILITYINFOHELPER_H__

#include "_Elastos_Droid_Content_Res_CCompatibilityInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CCompatibilityInfoHelper)
    , public Singleton
    , public ICompatibilityInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Compute the frame Rect for applications runs under compatibility mode.
     *
     * @param dm the display metrics used to compute the frame size.
     * @param orientation the orientation of the screen.
     * @param outRect the output parameter which will contain the result.
     * @return Returns the scaling factor for the window.
     */
    CARAPI ComputeCompatibleScaling(
        /* [in] */ IDisplayMetrics* dm,
        /* [in] */ IDisplayMetrics* outDm,
        /* [out] */ Float* scale);

    CARAPI GetDefault(
        /* [out] */ ICompatibilityInfo** info);

private:
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CCOMPATIBILITYINFOHELPER_H__
