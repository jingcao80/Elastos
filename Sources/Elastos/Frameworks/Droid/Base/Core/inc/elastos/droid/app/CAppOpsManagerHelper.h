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

#ifndef __ELASTOS_DROID_APP_CAPP_OPS_MANAGERHELPER_H__
#define __ELASTOS_DROID_APP_CAPP_OPS_MANAGERHELPER_H__

#include "_Elastos_Droid_App_CAppOpsManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::App::IIAppOpsService;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CAppOpsManagerHelper)
    , public Singleton
    , public IAppOpsManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * {@hide}
     */
    CARAPI StrOpToOp(
        /* [in] */ const String& op,
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI GetToken(
        /* [in] */ IIAppOpsService* service,
        /* [out] */ IBinder** binder);

    /**
     * Retrieve the op switch that controls the given operation.
     * @hide
     */
    CARAPI OpToSwitch(
        /* [in] */ Int32 op,
        /* [out] */ Int32* result);

    /**
     * Retrieve a non-localized name for the operation, for debugging output.
     * @hide
     */
    CARAPI OpToName(
        /* [in] */ Int32 op,
        /* [out] */ String* result);

    /**
     * Map a non-localized name for the operation back to the Op number
     * @hide
     */
    CARAPI NameToOp(
        /* [in] */ const String& name,
        /* [out] */ Int32* op);

    /**
     * Retrieve the permission associated with an operation, or null if there is not one.
     * @hide
     */
    CARAPI OpToPermission(
        /* [in] */ Int32 op,
        /* [out] */ String* result);

    /**
     * Retrieve the user restriction associated with an operation, or null if there is not one.
     * @hide
     */
    CARAPI OpToRestriction(
        /* [in] */ Int32 op,
        /* [out] */ String* result);

    /**
     * Retrieve whether the op allows the system (and system ui) to
     * bypass the user restriction.
     * @hide
     */
    CARAPI OpAllowSystemBypassRestriction(
        /* [in] */ Int32 op,
        /* [out] */ Boolean* result);

    /**
     * Retrieve the default mode for the operation.
     * @hide
     */
    CARAPI OpToDefaultMode(
        /* [in] */ Int32 op,
        /* [in] */ Boolean isStrict,
        /* [out] */ Int32* result);

    /**
     * Retrieve whether the op allows itself to be reset.
     * @hide
     */
    CARAPI OpAllowsReset(
        /* [in] */ Int32 op,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsStrictEnable(
        /* [out] */ Boolean* result);

    /**
     * Check if op in strict mode
     * @hide
     */
    CARAPI IsStrictOp(
        /* [in] */ Int32 code,
        /* [out] */ Boolean* isStrictOp);

    /** @hide */
    CARAPI StringToMode(
        /* [in] */ const String& permission,
        /* [out] */ Int32* mode);

    /** @hide */
    CARAPI StringOpToOp(
        /* [in] */ const String& op,
        /* [out] */ Int32* value);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPP_OPS_MANAGERHELPER_H__
