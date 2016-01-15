#ifndef __ELASTOS_DROID_OS_CUSERHANDLEHELPER_H__
#define __ELASTOS_DROID_OS_CUSERHANDLEHELPER_H__

#include "_Elastos_Droid_Os_CUserHandleHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IStringBuilder;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CUserHandleHelper)
    , public Singleton
    , public IUserHandleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Checks to see if the user id is the same for the two uids, i.e., they belong to the same
     * user.
     * @hide
     */
    CARAPI IsSameUser(
        /* [in] */ Int32 uid1,
        /* [in] */ Int32 uid2,
        /* [out] */ Boolean* isSameUser);

    /**
     * Checks to see if both uids are referring to the same app id, ignoring the user id part of the
     * uids.
     * @param uid1 uid to compare
     * @param uid2 other uid to compare
     * @return whether the appId is the same for both uids
     * @hide
     */
    CARAPI IsSameApp(
        /* [in] */ Int32 uid1,
        /* [in] */ Int32 uid2,
        /* [out] */ Boolean* isSameApp);

    CARAPI IsIsolated(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* isIsolated);

    CARAPI IsApp(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* isApp);

    /**
     * Returns the user id for a given uid.
     * @hide
     */
    CARAPI GetUserId(
        /* [in] */ Int32 uid,
        /* [out] */ Int32* userId);

    CARAPI GetCallingUserId(
        /* [out] */ Int32* userId);

    /**
     * Returns the uid that is composed from the userId and the appId.
     * @hide
     */
    CARAPI GetUid(
        /* [in] */ Int32 userId,
        /* [in] */ Int32 appId,
        /* [out] */ Int32* uid);

    /**
     * Returns the app id (or base uid) for a given uid, stripping out the user id from it.
     * @hide
     */
    CARAPI GetAppId(
        /* [in] */ Int32 uid,
        /* [out] */ Int32* appId);

    /**
     * Returns the shared app gid for a given uid or appId.
     * @hide
     */
    CARAPI GetSharedAppGid(
        /* [in] */ Int32 id,
        /* [out] */ Int32* appGid);

    /**
     * Generate a text representation of the uid, breaking out its individual
     * components -- user, app, isolated, etc.
     * @hide
     */
    CARAPI FormatUid(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Int32 uid);

    /**
     * Generate a text representation of the uid, breaking out its individual
     * components -- user, app, isolated, etc.
     * @hide
     */
    CARAPI FormatUid(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 uid);

    /**
     * Returns the user id of the current process
     * @return user id of the current process
     * @hide
     */
    CARAPI GetMyUserId(
        /* [out] */ Int32* myUserId);

    /**
     * Read a UserHandle from a Parcel that was previously written
     * with {@link #writeToParcel(UserHandle, Parcel)}, returning either
     * a null or new object as appropriate.
     *
     * @param in The Parcel from which to read the UserHandle
     * @return Returns a new UserHandle matching the previously written
     * object, or null if a null had been written.
     *
     * @see #writeToParcel(UserHandle, Parcel)
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in,
        /* [out] */ IUserHandle** h);

    /**
     * Write a UserHandle to a Parcel, handling null pointers.  Must be
     * read with {@link #readFromParcel(Parcel)}.
     *
     * @param h The UserHandle to be written.
     * @param out The Parcel in which the UserHandle will be placed.
     *
     * @see #readFromParcel(Parcel)
     */
    CARAPI WriteToParcel(
        /* [in] */ IUserHandle* h,
        /* [in] */ IParcel* out);

    /** @hide A user handle to indicate all users on the device */
    CARAPI GetALL(
        /* [out] */ IUserHandle** h);

    /** @hide A user handle to indicate the current user of the device */
    CARAPI GetCURRENT(
        /* [out] */ IUserHandle** h);

    /** @hide A user handle to indicate that we would like to send to the current
     *  user, but if this is calling from a user process then we will send it
     *  to the caller's user instead of failing wiht a security exception */
    CARAPI GetCURRENT_OR_SELF(
        /* [out] */ IUserHandle** h);

    /** @hide A user handle to indicate the primary/owner user of the device */
    CARAPI GetOWNER(
        /* [out] */ IUserHandle** h);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CUSERHANDLEHELPER_H__
