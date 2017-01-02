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

#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGEMANAGERHELPER_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGEMANAGERHELPER_H__

#include "_Elastos_Droid_Content_Pm_CPackageManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Pm::IPackageManagerHelper;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CPackageManagerHelper)
    , public Singleton
    , public IPackageManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDataDirForUser(
        /* [in] */ Int32 userId,
        /* [in] */ const String& packageName,
        /* [out] */ String* directory);

    /** {@hide} */
    CARAPI InstallStatusToString(
        /* [in] */ Int32 status,
        /* [in] */ const String& msg,
        /* [out] */ String* str);

    /** {@hide} */
    CARAPI InstallStatusToString(
        /* [in] */ Int32 status,
        /* [out] */ String* str);

    CARAPI InstallStatusToPublicStatus(
        /* [in] */ Int32 status,
        /* [out] */ Int32* value);

    /** {@hide} */
    CARAPI DeleteStatusToString(
        /* [in] */ Int32 status,
        /* [in] */ const String& msg,
        /* [out] */ String* str);

    /** {@hide} */
    CARAPI DeleteStatusToString(
        /* [in] */ Int32 status,
        /* [out] */ String* str);

    /** {@hide} */
    CARAPI DeleteStatusToPublicStatus(
        /* [in] */ Int32 status,
        /* [out] */ Int32* value);
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CSIGNATUREHELPER_H__
