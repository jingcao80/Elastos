#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CACTIONUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CACTIONUTILS_H__

#include "_Elastos_Droid_Internal_Utility_Cm_CActionUtils.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CActionUtils)
    , public Singleton
    , public IActionUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Kills the top most / most recent user application, but leaves out the launcher.
     * This is function governed by {@link Settings.Secure.KILL_APP_LONGPRESS_BACK}.
     *
     * @param context the current context, used to retrieve the package manager.
     * @param userId the ID of the currently active user
     * @return {@code true} when a user application was found and closed.
     */
    CARAPI KillForegroundApp(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    /**
     * Attempt to bring up the last activity in the stack before the current active one.
     *
     * @param context
     * @return whether an activity was found to switch to
     */
    CARAPI SwitchToLastApp(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CACTIONUTILS_H__
