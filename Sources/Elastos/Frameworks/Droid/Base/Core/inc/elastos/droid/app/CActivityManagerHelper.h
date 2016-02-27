
#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGERHELPER_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGERHELPER_H__

#include "_Elastos_Droid_App_CActivityManagerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActivityManagerHelper)
    , public Singleton
    , public IActivityManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()


    /** @hide */
    CARAPI StaticGetMemoryClass(
        /* [out] */ Int32* cls);

    /** @hide */
    CARAPI StaticGetLargeMemoryClass(
        /* [out] */ Int32* cls);

    /** @hide */
    CARAPI IsLowRamDeviceStatic(
        /* [out] */ Boolean* result);

    /**
     * Used by persistent processes to determine if they are running on a
     * higher-end device so should be okay using hardware drawing acceleration
     * (which tends to consume a lot more RAM).
     * @hide
     */
    CARAPI IsHighEndGfx(
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    CARAPI IsForcedHighEndGfx(
        /* [out] */ Boolean* result);

    /**
     * Return the maximum number of recents entries that we will maintain and show.
     * @hide
     */
    CARAPI GetMaxRecentTasksStatic(
        /* [out] */ Int32* result);

    /**
     * Return the default limit on the number of recents that an app can make.
     * @hide
     */
    CARAPI GetDefaultAppRecentsLimitStatic(
        /* [out] */ Int32* result);

    /**
     * Return the maximum limit on the number of recents that an app can make.
     * @hide
     */
    CARAPI GetMaxAppRecentsLimitStatic(
        /* [out] */ Int32* result);

    /**
     * Return global memory state information for the calling process.  This
     * does not fill in all fields of the {@link RunningAppProcessInfo}.  The
     * only fields that will be filled in are
     * {@link RunningAppProcessInfo#pid},
     * {@link RunningAppProcessInfo#uid},
     * {@link RunningAppProcessInfo#lastTrimLevel},
     * {@link RunningAppProcessInfo#importance},
     * {@link RunningAppProcessInfo#lru}, and
     * {@link RunningAppProcessInfo#importanceReasonCode}.
     */
    CARAPI GetMyMemoryState(
        /* [out] */ IActivityManagerRunningAppProcessInfo* outInfo);

    /**
     * Returns "true" if the user interface is currently being messed with
     * by a monkey.
     */
    CARAPI IsUserAMonkey(
        /* [out] */ Boolean* result);

    /**
     * Returns "true" if device is running in a test harness.
     */
    CARAPI IsRunningInTestHarness(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI CheckComponentPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 owningUid,
        /* [in] */ Boolean exported,
        /* [out] */ Int32* value);

    /** @hide */
    CARAPI CheckUidPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 uid,
        /* [out] */ Int32* value);

    /** @hide */
    CARAPI HandleIncomingUser(
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean allowAll,
        /* [in] */ Boolean requireFull,
        /* [in] */ const String& name,
        /* [in] */ const String& callerPackage,
        /* [out] */ Int32* value);

    /** @hide */
    CARAPI GetCurrentUser(
        /* [out] */ Int32* currentUser);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGERHELPER_H__
