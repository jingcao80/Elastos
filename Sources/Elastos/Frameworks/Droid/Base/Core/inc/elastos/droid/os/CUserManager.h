
#ifndef __ELASTOS_DROID_OS_CUSERMANAGER_H__
#define __ELASTOS_DROID_OS_CUSERMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Os_CUserManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Utility::IList;
using Elastos::Core::ICharSequence;


namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Manages users and user details on a multi-user system.
 */
CarClass(CUserManager)
    , public Object
    , public IUserManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** @hide */
    static CARAPI_(AutoPtr<IUserManager>) Get(
        /* [in] */ IContext* context);

    /** @hide */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIUserManager* service);

    /**
     * Returns whether the system supports multiple users.
     * @return true if multiple users can be created, false if it is a single user device.
     * @hide
     */
    static CARAPI_(Boolean) SupportsMultipleUsers();

    /**
     * Returns the user handle for the user that the calling process is running on.
     *
     * @return the user handle of the user making this call.
     * @hide
     */
    CARAPI GetUserHandle(
        /* [out] */ Int32* userHandle);

    /**
     * Returns the user name of the user making this call.  This call is only
     * available to applications on the system image; it requires the
     * MANAGE_USERS permission.
     * @return the user name
     */
    CARAPI GetUserName(
        /* [out] */ String* userName);

    /**
     * Used to determine whether the user making this call is subject to
     * teleportations.
     *
     * <p>As of {@link android.os.Build.VERSION_CODES#LOLLIPOP}, this method can
     * now automatically identify goats using advanced goat recognition technology.</p>
     *
     * @return Returns true if the user making this call is a goat.
     */
    CARAPI IsUserAGoat(
        /* [out] */ Boolean* result);

    /**
     * Used to check if the user making this call is linked to another user. Linked users may have
     * a reduced number of available apps, app restrictions and account restrictions.
     * @return whether the user making this call is a linked user
     * @hide
     */
    CARAPI IsLinkedUser(
        /* [out] */ Boolean* result);

    /**
     * Checks if the calling app is running as a guest user.
     * @return whether the caller is a guest user.
     * @hide
     */
    CARAPI IsGuestUser(
        /* [out] */ Boolean* result);

    /**
     * Checks if the calling app is running in a managed profile.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     *
     * @return whether the caller is in a managed profile.
     * @hide
     */
    //@SystemApi
    CARAPI IsManagedProfile(
        /* [out] */ Boolean* result);

    /**
     * Return whether the given user is actively running.  This means that
     * the user is in the "started" state, not "stopped" -- it is currently
     * allowed to run code through scheduled alarms, receiving broadcasts,
     * etc.  A started user may be either the current foreground user or a
     * background user; the result here does not distinguish between the two.
     * @param user The user to retrieve the running state for.
     */
    CARAPI IsUserRunning(
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    /**
     * Return whether the given user is actively running <em>or</em> stopping.
     * This is like {@link #isUserRunning(UserHandle)}, but will also return
     * true if the user had been running but is in the process of being stopped
     * (but is not yet fully stopped, and still running some code).
     * @param user The user to retrieve the running state for.
     */
    CARAPI IsUserRunningOrStopping(
        /* [in] */ IUserHandle* user,
        /* [out] */ Boolean* result);

    /**
     * Returns the UserInfo object describing a specific user.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     * @param userHandle the user handle of the user whose information is being requested.
     * @return the UserInfo object for a specific user.
     * @hide
     */
    CARAPI GetUserInfo(
        /* [in] */ Int32 userHandle,
        /* [out] */ IUserInfo** userInfo);

    /**
     * Returns the user-wide restrictions imposed on this user.
     * @return a Bundle containing all the restrictions.
     */
    CARAPI GetUserRestrictions(
        /* [out] */ IBundle** bundle);

    /**
     * Returns the user-wide restrictions imposed on the user specified by <code>userHandle</code>.
     * @param userHandle the UserHandle of the user for whom to retrieve the restrictions.
     * @return a Bundle containing all the restrictions.
     */
    CARAPI GetUserRestrictions(
        /* [in] */ IUserHandle* user,
        /* [out] */ IBundle** bundle);

    /**
     * Sets all the user-wide restrictions for this user.
     * Requires the MANAGE_USERS permission.
     * @param restrictions the Bundle containing all the restrictions.
     * @deprecated use {@link android.app.admin.DevicePolicyManager#addUserRestriction(
     * android.content.ComponentName, String)} or
     * {@link android.app.admin.DevicePolicyManager#clearUserRestriction(
     * android.content.ComponentName, String)} instead.
     */
    //@Deprecated
    CARAPI SetUserRestrictions(
        /* [in] */ IBundle* bundle);

    /**
     * Sets all the user-wide restrictions for the specified user.
     * Requires the MANAGE_USERS permission.
     * @param restrictions the Bundle containing all the restrictions.
     * @param userHandle the UserHandle of the user for whom to set the restrictions.
     * @deprecated use {@link android.app.admin.DevicePolicyManager#addUserRestriction(
     * android.content.ComponentName, String)} or
     * {@link android.app.admin.DevicePolicyManager#clearUserRestriction(
     * android.content.ComponentName, String)} instead.
     */
    //@Deprecated
    CARAPI SetUserRestrictions(
        /* [in] */ IBundle* bundle,
        /* [in] */ IUserHandle* userHandle);

    /**
     * Sets the value of a specific restriction.
     * Requires the MANAGE_USERS permission.
     * @param key the key of the restriction
     * @param value the value for the restriction
     * @deprecated use {@link android.app.admin.DevicePolicyManager#addUserRestriction(
     * android.content.ComponentName, String)} or
     * {@link android.app.admin.DevicePolicyManager#clearUserRestriction(
     * android.content.ComponentName, String)} instead.
     */
    //@Deprecated
    CARAPI SetUserRestriction(
        /* [in] */ const String& key,
        /* [in] */ Boolean value);

    /**
     * @hide
     * Sets the value of a specific restriction on a specific user.
     * Requires the MANAGE_USERS permission.
     * @param key the key of the restriction
     * @param value the value for the restriction
     * @param userHandle the user whose restriction is to be changed.
     * @deprecated use {@link android.app.admin.DevicePolicyManager#addUserRestriction(
     * android.content.ComponentName, String)} or
     * {@link android.app.admin.DevicePolicyManager#clearUserRestriction(
     * android.content.ComponentName, String)} instead.
     */
    //@Deprecated
    CARAPI SetUserRestriction(
        /* [in] */ const String& key,
        /* [in] */ Boolean value,
        /* [in] */ IUserHandle* userHandle);

    /**
     * Returns whether the current user has been disallowed from performing certain actions
     * or setting certain settings.
     *
     * @param restrictionKey The string key representing the restriction.
     * @return {@code true} if the current user has the given restriction, {@code false} otherwise.
     */
    CARAPI HasUserRestriction(
        /* [in] */ const String& restrictionKey,
        /* [out] */ Boolean* result);

    /**
     * @hide
     * Returns whether the given user has been disallowed from performing certain actions
     * or setting certain settings.
     * @param restrictionKey the string key representing the restriction
     * @param userHandle the UserHandle of the user for whom to retrieve the restrictions.
     */
    CARAPI HasUserRestriction(
        /* [in] */ const String& restrictionKey,
        /* [in] */ IUserHandle* handle,
        /* [out] */ Boolean* result);

    /**
     * Return the serial number for a user.  This is a device-unique
     * number assigned to that user; if the user is deleted and then a new
     * user created, the new users will not be given the same serial number.
     * @param user The user whose serial number is to be retrieved.
     * @return The serial number of the given user; returns -1 if the
     * given UserHandle does not exist.
     * @see #getUserForSerialNumber(long)
     */
    CARAPI GetSerialNumberForUser(
        /* [in] */ IUserHandle* user,
        /* [out] */ Int64* number);

    /**
     * Return the user associated with a serial number previously
     * returned by {@link #getSerialNumberForUser(UserHandle)}.
     * @param serialNumber The serial number of the user that is being
     * retrieved.
     * @return Return the user associated with the serial number, or null
     * if there is not one.
     * @see #getSerialNumberForUser(UserHandle)
     */
    CARAPI GetUserForSerialNumber(
        /* [in] */ Int64 serialNumber,
        /* [out] */ IUserHandle** user);

    /**
     * Creates a user with the specified name and options.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     *
     * @param name the user's name
     * @param flags flags that identify the type of user and other properties.
     * @see UserInfo
     *
     * @return the UserInfo object for the created user, or null if the user could not be created.
     * @hide
     */
    CARAPI CreateUser(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [out] */ IUserInfo** userInfo);

    /**
     * Creates a guest user and configures it.
     * @param context an application context
     * @param name the name to set for the user
     * @hide
     */
    CARAPI CreateGuest(
        /* [in] */ IContext* context,
        /* [in] */ const String& name,
        /* [out] */ IUserInfo** userInfo);

    /**
     * Creates a secondary user with the specified name and options and configures it with default
     * restrictions.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     *
     * @param name the user's name
     * @param flags flags that identify the type of user and other properties.
     * @see UserInfo
     *
     * @return the UserInfo object for the created user, or null if the user could not be created.
     * @hide
     */
    CARAPI CreateSecondaryUser(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [out] */ IUserInfo** userInfo);

    static CARAPI AddDefaultUserRestrictions(
        /* [in] */ IBundle* restrictions);

    /**
     * Creates a user with the specified name and options as a profile of another user.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     *
     * @param name the user's name
     * @param flags flags that identify the type of user and other properties.
     * @see UserInfo
     * @param userHandle new user will be a profile of this use.
     *
     * @return the UserInfo object for the created user, or null if the user could not be created.
     * @hide
     */
    CARAPI CreateProfileForUser(
        /* [in] */ const String& name,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userHandle,
        /* [out] */ IUserInfo** userInfo);

    /**
     * Creates a user with the specified name and options as a profile of another user.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     *
     * @param name the user's name
     * @param flags flags that identify the type of user and other properties.
     * @see UserInfo
     * @param userHandle new user will be a profile of this use.
     *
     * @return the UserInfo object for the created user, or null if the user could not be created.
     * @hide
     */
    CARAPI MarkGuestForDeletion(
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Sets the user as enabled, if such an user exists.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     * Note that the default is true, it's only that managed profiles might not be enabled.
     *
     * @param userHandle the id of the profile to enable
     * @hide
     */
    CARAPI SetUserEnabled(
        /* [in] */ Int32 userHandle);

    /**
     * Return the number of users currently created on the device.
     */
    CARAPI GetUserCount(
        /* [out] */ Int32* count);

    /**
     * Returns information for all users on this device.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     * @return the list of users that were created.
     * @hide
     */
    CARAPI GetUsers(
        /* [out] */ IList** users);

    /**
     * Checks whether it's possible to add more users. Caller must hold the MANAGE_USERS
     * permission.
     *
     * @return true if more users can be added, false if limit has been reached.
     * @hide
     */
    CARAPI CanAddMoreUsers(
        /* [out] */ Boolean* result);

    /**
     * Returns list of the profiles of userHandle including
     * userHandle itself.
     * Note that this returns both enabled and not enabled profiles. See
     * {@link #getUserProfiles()} if you need only the enabled ones.
     *
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     * @param userHandle profiles of this user will be returned.
     * @return the list of profiles.
     * @hide
     */
    CARAPI GetProfiles(
        /* [in] */ Int32 userHandle,
        /* [out] */ IList** list);

    /**
     * Returns a list of UserHandles for profiles associated with the user that the calling process
     * is running on, including the user itself.
     *
     * @return A non-empty list of UserHandles associated with the calling user.
     */
    CARAPI GetUserProfiles(
        /* [out] */ IList** list);

    /**
     * Returns the parent of the profile which this method is called from
     * or null if called from a user that is not a profile.
     *
     * @hide
     */
    CARAPI GetProfileParent(
        /* [in] */ Int32 userHandle,
        /* [out] */ IUserInfo** userInfo);

    /**
     * If the target user is a managed profile of the calling user or the caller
     * is itself a managed profile, then this returns a badged copy of the given
     * icon to be able to distinguish it from the original icon. For badging an
     * arbitrary drawable use {@link #getBadgedDrawableForUser(
     * android.graphics.drawable.Drawable, UserHandle, android.graphics.Rect, int)}.
     * <p>
     * If the original drawable is a BitmapDrawable and the backing bitmap is
     * mutable as per {@link android.graphics.Bitmap#isMutable()}, the bading
     * is performed in place and the original drawable is returned.
     * </p>
     *
     * @param icon The icon to badge.
     * @param user The target user.
     * @return A drawable that combines the original icon and a badge as
     *         determined by the system.
     * @removed
     */
    CARAPI GetBadgedIconForUser(
        /* [in] */ IDrawable* icon,
        /* [in] */ IUserHandle* user,
        /* [out] */ IDrawable** drawable);

    /**
     * If the target user is a managed profile of the calling user or the caller
     * is itself a managed profile, then this returns a badged copy of the given
     * drawable allowing the user to distinguish it from the original drawable.
     * The caller can specify the location in the bounds of the drawable to be
     * badged where the badge should be applied as well as the density of the
     * badge to be used.
     * <p>
     * If the original drawable is a BitmapDrawable and the backing bitmap is
     * mutable as per {@link android.graphics.Bitmap#isMutable()}, the bading
     * is performed in place and the original drawable is returned.
     * </p>
     *
     * @param badgedDrawable The drawable to badge.
     * @param user The target user.
     * @param badgeLocation Where in the bounds of the badged drawable to place
     *         the badge. If not provided, the badge is applied on top of the entire
     *         drawable being badged.
     * @param badgeDensity The optional desired density for the badge as per
     *         {@link android.util.DisplayMetrics#densityDpi}. If not provided,
     *         the density of the display is used.
     * @return A drawable that combines the original drawable and a badge as
     *         determined by the system.
     * @removed
     */
    CARAPI GetBadgedDrawableForUser(
        /* [in] */ IDrawable* badgedDrawable,
        /* [in] */ IUserHandle* user,
        /* [in] */ IRect* badgeLocation,
        /* [in] */ Int32 badgeDensity,
        /* [out] */ IDrawable** drawable);

    /**
     * If the target user is a managed profile of the calling user or the caller
     * is itself a managed profile, then this returns a copy of the label with
     * badging for accessibility services like talkback. E.g. passing in "Email"
     * and it might return "Work Email" for Email in the work profile.
     *
     * @param label The label to change.
     * @param user The target user.
     * @return A label that combines the original label and a badge as
     *         determined by the system.
     * @removed
     */
    CARAPI GetBadgedLabelForUser(
        /* [in] */ ICharSequence* label,
        /* [in] */ IUserHandle* user,
        /* [out] */ ICharSequence** csq);

    /**
     * Returns information for all users on this device. Requires
     * {@link android.Manifest.permission#MANAGE_USERS} permission.
     *
     * @param excludeDying specify if the list should exclude users being
     *            removed.
     * @return the list of users that were created.
     * @hide
     */

    CARAPI GetUsers(
        /* [in] */ Boolean excludeDying,
        /* [out] */ IList** users);

    /**
     * Removes a user and all associated data.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     * @param userHandle the integer handle of the user, where 0 is the primary user.
     * @hide
     */
    CARAPI RemoveUser(
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

   /**
     * Updates the user's name.
     * Requires {@link android.Manifest.permission#MANAGE_USERS} permission.
     *
     * @param userHandle the user's integer handle
     * @param name the new name for the user
     * @hide
     */
    CARAPI SetUserName(
        /* [in] */ Int32 userHandle,
        /* [in] */ const String& name);

    /**
     * Sets the user's photo.
     * @param userHandle the user for whom to change the photo.
     * @param icon the bitmap to set as the photo.
     * @hide
     */
    CARAPI SetUserIcon(
        /* [in] */ Int32 userHandle,
        /* [in] */ IBitmap* icon);

    /**
     * Returns a file descriptor for the user's photo. PNG data can be read from this file.
     * @param userHandle the user whose photo we want to read.
     * @return a {@link Bitmap} of the user's photo, or null if there's no photo.
     * @hide
     */
    CARAPI GetUserIcon(
        /* [in] */ Int32 userHandle,
        /* [out] */ IBitmap** icon);

    /**
     * Returns the maximum number of users that can be created on this device. A return value
     * of 1 means that it is a single user device.
     * @hide
     * @return a value greater than or equal to 1
     */
    static CARAPI_(Int32) GetMaxSupportedUsers();

    /**
     * Returns true if the user switcher should be shown, this will be if there
     * are multiple users that aren't managed profiles.
     * @hide
     * @return true if user switcher should be shown.
     */
    CARAPI IsUserSwitcherEnabled(
        /* [out] */ Boolean* result);

    /**
     * Returns a serial number on this device for a given userHandle. User handles can be recycled
     * when deleting and creating users, but serial numbers are not reused until the device is wiped.
     * @param userHandle
     * @return a serial number associated with that user, or -1 if the userHandle is not valid.
     * @hide
     */
    CARAPI GetUserSerialNumber(
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* number);

    /**
     * Returns a userHandle on this device for a given user serial number. User handles can be
     * recycled when deleting and creating users, but serial numbers are not reused until the device
     * is wiped.
     * @param userSerialNumber
     * @return the userHandle associated with that user serial number, or -1 if the serial number
     * is not valid.
     * @hide
     */
    CARAPI GetUserHandle(
        /* [in] */ Int32 userSerialNumber,
        /* [out] */ Int32* userHandle);

    /**
     * Returns a Bundle containing any saved application restrictions for this user, for the
     * given package name. Only an application with this package name can call this method.
     * @param packageName the package name of the calling application
     * @return a Bundle with the restrictions as key/value pairs, or null if there are no
     * saved restrictions. The values can be of type Boolean, String or String[], depending
     * on the restriction type, as defined by the application.
     */
    CARAPI GetApplicationRestrictions(
        /* [in] */ const String& packageName,
        /* [out] */ IBundle** bundle);

    /**
     * @hide
     */
    CARAPI GetApplicationRestrictions(
        /* [in] */ const String& packageName,
        /* [in] */ IUserHandle* user,
        /* [out] */ IBundle** bundle);

    /**
     * @hide
     */
    CARAPI SetApplicationRestrictions(
        /* [in] */ const String& packageName,
        /* [in] */ IBundle* restrictions,
        /* [in] */ IUserHandle* user);

    /**
     * Sets a new challenge PIN for restrictions. This is only for use by pre-installed
     * apps and requires the MANAGE_USERS permission.
     * @param newPin the PIN to use for challenge dialogs.
     * @return Returns true if the challenge PIN was set successfully.
     */
    CARAPI SetRestrictionsChallenge(
        /* [in] */ const String& newPin,
        /* [out] */ Boolean* result);

    /**
     * @hide
     * @param pin The PIN to verify, or null to get the number of milliseconds to wait for before
     * allowing the user to enter the PIN.
     * @return Returns a positive number (including zero) for how many milliseconds before
     * you can accept another PIN, when the input is null or the input doesn't match the saved PIN.
     * Returns {@link #PIN_VERIFICATION_SUCCESS} if the input matches the saved PIN. Returns
     * {@link #PIN_VERIFICATION_FAILED_NOT_SET} if there is no PIN set.
     */
    CARAPI CheckRestrictionsChallenge(
        /* [in] */ const String& pin,
        /* [out] */ Int32* result);

    /**
     * @hide
     * Checks whether the user has restrictions that are PIN-protected. An application that
     * participates in restrictions can check if the owner has requested a PIN challenge for
     * any restricted operations. If there is a PIN in effect, the application should launch
     * the PIN challenge activity {@link android.content.Intent#ACTION_RESTRICTIONS_CHALLENGE}.
     * @see android.content.Intent#ACTION_RESTRICTIONS_CHALLENGE
     * @return whether a restrictions PIN is in effect.
     */
    CARAPI HasRestrictionsChallenge(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI RemoveRestrictions();

    /**
     * @hide
     * Set restrictions that should apply to any future guest user that's created.
     */
    CARAPI SetDefaultGuestRestrictions(
        /* [in] */ IBundle* restrictions);

    /**
     * @hide
     * Gets the default guest restrictions.
     */
    CARAPI GetDefaultGuestRestrictions(
        /* [out] */ IBundle** bundle);

private:
    static String TAG;
    static AutoPtr<IUserManager> sInstance;

    AutoPtr<IIUserManager> mService;
    IContext* mContext;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CUSERMANAGER_H__

