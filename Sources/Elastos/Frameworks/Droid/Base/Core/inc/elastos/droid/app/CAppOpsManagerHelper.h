
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
        /* [out] */ Int32* result);

    /**
     * Retrieve whether the op allows itself to be reset.
     * @hide
     */
    CARAPI OpAllowsReset(
        /* [in] */ Int32 op,
        /* [out] */ Boolean* result);

    CARAPI IsStrictEnable(
        /* [out] */ Boolean* result);

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
