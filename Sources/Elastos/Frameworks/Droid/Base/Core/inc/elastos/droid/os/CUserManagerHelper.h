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

#ifndef __ELASTOS_DROID_OS_CUSERMANAGERHELPER_H__
#define __ELASTOS_DROID_OS_CUSERMANAGERHELPER_H__

#include "_Elastos_Droid_Os_CUserManagerHelper.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CUserManagerHelper)
    , public Singleton
    , public IUserManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** @hide */
    CARAPI Get(
        /* [in] */ IContext* ctx,
        /* [out] */ IUserManager** userManager);

    /**
     * Returns whether the system supports multiple users.
     * @return true if multiple users can be created, false if it is a single user device.
     * @hide
     */
    CARAPI SupportsMultipleUsers(
        /* [out] */ Boolean* result);

    /**
     * Returns the maximum number of users that can be created on this device. A return value
     * of 1 means that it is a single user device.
     * @hide
     * @return a value greater than or equal to 1
     */
    CARAPI GetMaxSupportedUsers(
        /* [out] */ Int32* number);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CUSERMANAGERHELPER_H__
