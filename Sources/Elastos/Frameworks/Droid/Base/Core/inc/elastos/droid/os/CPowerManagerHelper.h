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

#ifndef __ELASTOS_DROID_OS_CPOWERMANAGERHELPER_H__
#define __ELASTOS_DROID_OS_CPOWERMANAGERHELPER_H__

#include "_Elastos_Droid_Os_CPowerManagerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CPowerManagerHelper)
    , public Singleton
    , public IPowerManagerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns true if the twilight service should be used to adjust screen brightness
     * policy.  This setting is experimental and disabled by default.
     * @hide
     */
    CARAPI UseTwilightAdjustmentFeature(
        /* [out] */ Boolean* result);

    CARAPI ValidateWakeLockParameters(
        /* [in] */ Int32 levelAndFlags,
        /* [in] */ const String& tag);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CPOWERMANAGERHELPER_H__
