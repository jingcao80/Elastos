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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSUTILS_H__

#include "_Elastos_Droid_Internal_Utility_Cm_CQSUtils.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CQSUtils)
    , public Singleton
    , public IQSUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAvailableTiles(
        /* [in] */ IContext* context,
        /* [out] */ IList** result);

    CARAPI GetDefaultTiles(
        /* [in] */ IContext* context,
        /* [out] */ IList** result);

    CARAPI GetDefaultTilesAsString(
        /* [in] */ IContext* context,
        /* [out] */ String* result);

    CARAPI DeviceSupportsLte(
        /* [in] */ IContext* ctx,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsDdsSupported(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsMobileData(
        /* [in] */ IContext* ctx,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsBluetooth(
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsNfc(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsFlashLight(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsCompass(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSUTILS_H__
