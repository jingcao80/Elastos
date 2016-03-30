#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_ACTIONUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_ACTIONUTILS_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

class ActionUtils
{
public:
    /**
     * Kills the top most / most recent user application, but leaves out the launcher.
     * This is function governed by {@link Settings.Secure.KILL_APP_LONGPRESS_BACK}.
     *
     * @param context the current context, used to retrieve the package manager.
     * @param userId the ID of the currently active user
     * @return {@code true} when a user application was found and closed.
     */
    static CARAPI KillForegroundApp(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    /**
     * Attempt to bring up the last activity in the stack before the current active one.
     *
     * @param context
     * @return whether an activity was found to switch to
     */
    static CARAPI SwitchToLastApp(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(Boolean) KillForegroundAppInternal(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    static CARAPI_(Boolean) SwitchToLastAppInternal(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    static CARAPI_(AutoPtr<IActivityManagerRecentTaskInfo>) GetLastTask(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    static CARAPI_(String) ResolveCurrentLauncherPackage(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

private:
    const static Boolean DEBUG;
    const static String TAG;
    const static String SYSTEMUI_PACKAGE;
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_ACTIONUTILS_H__
