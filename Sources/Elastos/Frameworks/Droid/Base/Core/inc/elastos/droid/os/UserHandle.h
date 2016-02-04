
#ifndef __ELASTOS_DROID_OS_USERHANDLE_H__
#define __ELASTOS_DROID_OS_USERHANDLE_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::IStringBuilder;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Os {

class ECO_PUBLIC UserHandle
    : public Object
    , public IUserHandle
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    UserHandle();

    UserHandle(
        /* [in] */ Int32 h);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 h);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetIdentifier(
        /* [out] */ Int32* id);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* equals);

    CARAPI Equals(
        /* [in] */ IUserHandle* other,
        /* [out] */ Boolean* equals);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    /**
     * Write a UserHandle to a Parcel, handling null pointers.  Must be
     * read with {@link #readFromParcel(Parcel)}.
     *
     * @param h The UserHandle to be written.
     * @param out The Parcel in which the UserHandle will be placed.
     *
     * @see #readFromParcel(Parcel)
     */
    static CARAPI WriteToParcel(
        /* [in] */ IUserHandle* h,
        /* [in] */ IParcel* parcel);

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
    static CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel,
        /* [out] */ IUserHandle** h);

    /**
     * Checks to see if the user id is the same for the two uids, i.e., they belong to the same
     * user.
     * @hide
     */
    static CARAPI_(Boolean) IsSameUser(
        /* [in] */ Int32 uid1,
        /* [in] */ Int32 uid2);

    /**
     * Checks to see if both uids are referring to the same app id, ignoring the user id part of the
     * uids.
     * @param uid1 uid to compare
     * @param uid2 other uid to compare
     * @return whether the appId is the same for both uids
     * @hide
     */
    static CARAPI_(Boolean) IsSameApp(
        /* [in] */ Int32 uid1,
        /* [in] */ Int32 uid2);

    static CARAPI_(Boolean) IsIsolated(
        /* [in] */ Int32 uid);

    static CARAPI_(Boolean) IsApp(
        /* [in] */ Int32 uid);

    /**
     * Returns the user id for a given uid.
     * @hide
     */
    static CARAPI_(Int32) GetUserId(
        /* [in] */ Int32 uid);

    static CARAPI_(Int32) GetCallingUserId();

    static CARAPI_(AutoPtr<IUserHandle>) GetCallingUserHandle();

    /**
     * Returns the uid that is composed from the userId and the appId.
     * @hide
     */
    static CARAPI_(Int32) GetUid(
        /* [in] */ Int32 userId,
        /* [in] */ Int32 appId);

    /**
     * Returns the app id (or base uid) for a given uid, stripping out the user id from it.
     * @hide
     */
    static CARAPI_(Int32) GetAppId(
        /* [in] */ Int32 uid);

    /**
     * Returns the shared app gid for a given uid or appId.
     * @hide
     */
    static CARAPI_(Int32) GetSharedAppGid(
        /* [in] */ Int32 id);

    /**
     * Generate a text representation of the uid, breaking out its individual
     * components -- user, app, isolated, etc.
     * @hide
     */
    static CARAPI FormatUid(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Int32 uid);

    /**
     * Generate a text representation of the uid, breaking out its individual
     * components -- user, app, isolated, etc.
     * @hide
     */
    static CARAPI FormatUid(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 uid);

    /**
     * Returns the user id of the current process
     * @return user id of the current process
     * @hide
     */
    static CARAPI_(Int32) GetMyUserId();

public:
    /** @hide A user handle to indicate all users on the device */
    static const AutoPtr<IUserHandle> ALL;

    /** @hide A user handle to indicate the current user of the device */
    static const AutoPtr<IUserHandle> CURRENT;

    /** @hide A user handle to indicate that we would like to send to the current
     *  user, but if this is calling from a user process then we will send it
     *  to the caller's user instead of failing wiht a security exception */
    static const AutoPtr<IUserHandle> CURRENT_OR_SELF;

    /** @hide A user handle to indicate the primary/owner user of the device */
    static const AutoPtr<IUserHandle> OWNER;

private:
    Int32 mHandle;

    // private static final SparseArray<UserHandle> userHandles = new SparseArray<UserHandle>();
    ECO_LOCAL static AutoPtr<ISparseArray> mUserHandles;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_USERHANDLE_H__
