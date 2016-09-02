
#ifndef __ELASTOS_DROID_APP_ADMIN_CDEVICEPOLICYMANAGER_H__
#define __ELASTOS_DROID_APP_ADMIN_CDEVICEPOLICYMANAGER_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_Admin_CDevicePolicyManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

CarClass(CDevicePolicyManager)
    , public Object
    , public IDevicePolicyManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    static CARAPI Create(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [out] */ IDevicePolicyManager** policymanager);

    /**
     * Return true if the given administrator component is currently
     * active (enabled) in the system.
     */
    CARAPI IsAdminActive(
        /* [in] */ IComponentName* who,
        /* [out] */ Boolean* isAdminActive);

    CARAPI IsAdminActiveAsUser(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* isAdminActive);

    /**
     * Return a list of all currently active device administrator's component
     * names.  Note that if there are no administrators than null may be
     * returned.
     */
    CARAPI GetActiveAdmins(
        /* [out] */ IList** admins);

    CARAPI GetActiveAdminsAsUser(
        /* [in] */ Int32 userId,
        /* [out] */ IList** admins);

    /**
     * Used by package administration code to determine if a package can be stopped
     * or uninstalled.
     * @hide
     */
    CARAPI PackageHasActiveAdmins(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* hasAdmins);

    /**
     * Remove a current administration component.  This can only be called
     * by the application that owns the administration component; if you
     * try to remove someone else's component, a security exception will be
     * thrown.
     */
    CARAPI RemoveActiveAdmin(
        /* [in] */ IComponentName* who);

    /**
     * Returns true if an administrator has been granted a particular device policy.  This can
     * be used to check if the administrator was activated under an earlier set of policies,
     * but requires additional policies after an upgrade.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.  Must be
     * an active administrator, or an exception will be thrown.
     * @param usesPolicy Which uses-policy to check, as defined in {@link DeviceAdminInfo}.
     */
    CARAPI HasGrantedPolicy(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 usesPolicy,
        /* [out] */ Boolean* hasGrant);

    /**
     * Called by an application that is administering the device to set the
     * password restrictions it is imposing.  After setting this, the user
     * will not be able to enter a new password that is not at least as
     * restrictive as what has been set.  Note that the current password
     * will remain until the user has set a new one, so the change does not
     * take place immediately.  To prompt the user for a new password, use
     * {@link #ACTION_SET_NEW_PASSWORD} after setting this value.
     *
     * <p>Quality constants are ordered so that higher values are more restrictive;
     * thus the highest requested quality constant (between the policy set here,
     * the user's preference, and any other considerations) is the one that
     * is in effect.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param quality The new desired quality.  One of
     * {@link #PASSWORD_QUALITY_UNSPECIFIED}, {@link #PASSWORD_QUALITY_SOMETHING},
     * {@link #PASSWORD_QUALITY_NUMERIC}, {@link #PASSWORD_QUALITY_ALPHABETIC},
     * {@link #PASSWORD_QUALITY_ALPHANUMERIC} or {@link #PASSWORD_QUALITY_COMPLEX}.
     */
    CARAPI SetPasswordQuality(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 quality);

    /**
     * Retrieve the current minimum password quality for all admins
     * or a particular one.
     * @param admin The name of the admin component to check, or null to aggregate
     * all admins.
     */
    CARAPI GetPasswordQuality(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetPasswordQuality(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
     * Called by an application that is administering the device to set the
     * minimum allowed password length.  After setting this, the user
     * will not be able to enter a new password that is not at least as
     * restrictive as what has been set.  Note that the current password
     * will remain until the user has set a new one, so the change does not
     * take place immediately.  To prompt the user for a new password, use
     * {@link #ACTION_SET_NEW_PASSWORD} after setting this value.  This
     * constraint is only imposed if the administrator has also requested either
     * {@link #PASSWORD_QUALITY_NUMERIC}, {@link #PASSWORD_QUALITY_ALPHABETIC}
     * {@link #PASSWORD_QUALITY_ALPHANUMERIC}, or {@link #PASSWORD_QUALITY_COMPLEX}
     * with {@link #setPasswordQuality}.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param length The new desired minimum password length.  A value of 0
     * means there is no restriction.
     */
    CARAPI SetPasswordMinimumLength(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 length);

    /**
     * Retrieve the current minimum password length for all admins
     * or a particular one.
     * @param admin The name of the admin component to check, or null to aggregate
     * all admins.
     */
    CARAPI GetPasswordMinimumLength(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetPasswordMinimumLength(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
     * Called by an application that is administering the device to set the
     * minimum number of upper case letters required in the password. After
     * setting this, the user will not be able to enter a new password that is
     * not at least as restrictive as what has been set. Note that the current
     * password will remain until the user has set a new one, so the change does
     * not take place immediately. To prompt the user for a new password, use
     * {@link #ACTION_SET_NEW_PASSWORD} after setting this value. This
     * constraint is only imposed if the administrator has also requested
     * {@link #PASSWORD_QUALITY_COMPLEX} with {@link #setPasswordQuality}. The
     * default value is 0.
     * <p>
     * The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated
     *            with.
     * @param length The new desired minimum number of upper case letters
     *            required in the password. A value of 0 means there is no
     *            restriction.
     */
    CARAPI SetPasswordMinimumUpperCase(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 length);

    /**
     * Retrieve the current number of upper case letters required in the
     * password for all admins or a particular one. This is the same value as
     * set by {#link {@link #setPasswordMinimumUpperCase(ComponentName, int)}
     * and only applies when the password quality is
     * {@link #PASSWORD_QUALITY_COMPLEX}.
     *
     * @param admin The name of the admin component to check, or null to
     *            aggregate all admins.
     * @return The minimum number of upper case letters required in the
     *         password.
     */
    CARAPI GetPasswordMinimumUpperCase(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetPasswordMinimumUpperCase(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
     * Called by an application that is administering the device to set the
     * minimum number of lower case letters required in the password. After
     * setting this, the user will not be able to enter a new password that is
     * not at least as restrictive as what has been set. Note that the current
     * password will remain until the user has set a new one, so the change does
     * not take place immediately. To prompt the user for a new password, use
     * {@link #ACTION_SET_NEW_PASSWORD} after setting this value. This
     * constraint is only imposed if the administrator has also requested
     * {@link #PASSWORD_QUALITY_COMPLEX} with {@link #setPasswordQuality}. The
     * default value is 0.
     * <p>
     * The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated
     *            with.
     * @param length The new desired minimum number of lower case letters
     *            required in the password. A value of 0 means there is no
     *            restriction.
     */
    CARAPI SetPasswordMinimumLowerCase(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 length);

    /**
     * Retrieve the current number of lower case letters required in the
     * password for all admins or a particular one. This is the same value as
     * set by {#link {@link #setPasswordMinimumLowerCase(ComponentName, int)}
     * and only applies when the password quality is
     * {@link #PASSWORD_QUALITY_COMPLEX}.
     *
     * @param admin The name of the admin component to check, or null to
     *            aggregate all admins.
     * @return The minimum number of lower case letters required in the
     *         password.
     */
    CARAPI GetPasswordMinimumLowerCase(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetPasswordMinimumLowerCase(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
     * Called by an application that is administering the device to set the
     * minimum number of letters required in the password. After setting this,
     * the user will not be able to enter a new password that is not at least as
     * restrictive as what has been set. Note that the current password will
     * remain until the user has set a new one, so the change does not take
     * place immediately. To prompt the user for a new password, use
     * {@link #ACTION_SET_NEW_PASSWORD} after setting this value. This
     * constraint is only imposed if the administrator has also requested
     * {@link #PASSWORD_QUALITY_COMPLEX} with {@link #setPasswordQuality}. The
     * default value is 1.
     * <p>
     * The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated
     *            with.
     * @param length The new desired minimum number of letters required in the
     *            password. A value of 0 means there is no restriction.
     */
    CARAPI SetPasswordMinimumLetters(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 length);

    /**
     * Retrieve the current number of letters required in the password for all
     * admins or a particular one. This is the same value as
     * set by {#link {@link #setPasswordMinimumLetters(ComponentName, int)}
     * and only applies when the password quality is
     * {@link #PASSWORD_QUALITY_COMPLEX}.
     *
     * @param admin The name of the admin component to check, or null to
     *            aggregate all admins.
     * @return The minimum number of letters required in the password.
     */
    CARAPI GetPasswordMinimumLetters(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetPasswordMinimumLetters(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
     * Called by an application that is administering the device to set the
     * minimum number of numerical digits required in the password. After
     * setting this, the user will not be able to enter a new password that is
     * not at least as restrictive as what has been set. Note that the current
     * password will remain until the user has set a new one, so the change does
     * not take place immediately. To prompt the user for a new password, use
     * {@link #ACTION_SET_NEW_PASSWORD} after setting this value. This
     * constraint is only imposed if the administrator has also requested
     * {@link #PASSWORD_QUALITY_COMPLEX} with {@link #setPasswordQuality}. The
     * default value is 1.
     * <p>
     * The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated
     *            with.
     * @param length The new desired minimum number of numerical digits required
     *            in the password. A value of 0 means there is no restriction.
     */
    CARAPI SetPasswordMinimumNumeric(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 length);

    /**
     * Retrieve the current number of numerical digits required in the password
     * for all admins or a particular one. This is the same value as
     * set by {#link {@link #setPasswordMinimumNumeric(ComponentName, int)}
     * and only applies when the password quality is
     * {@link #PASSWORD_QUALITY_COMPLEX}.
     *
     * @param admin The name of the admin component to check, or null to
     *            aggregate all admins.
     * @return The minimum number of numerical digits required in the password.
     */
    CARAPI GetPasswordMinimumNumeric(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetPasswordMinimumNumeric(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
     * Called by an application that is administering the device to set the
     * minimum number of symbols required in the password. After setting this,
     * the user will not be able to enter a new password that is not at least as
     * restrictive as what has been set. Note that the current password will
     * remain until the user has set a new one, so the change does not take
     * place immediately. To prompt the user for a new password, use
     * {@link #ACTION_SET_NEW_PASSWORD} after setting this value. This
     * constraint is only imposed if the administrator has also requested
     * {@link #PASSWORD_QUALITY_COMPLEX} with {@link #setPasswordQuality}. The
     * default value is 1.
     * <p>
     * The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated
     *            with.
     * @param length The new desired minimum number of symbols required in the
     *            password. A value of 0 means there is no restriction.
     */
    CARAPI SetPasswordMinimumSymbols(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 length);

    /**
     * Retrieve the current number of symbols required in the password for all
     * admins or a particular one. This is the same value as
     * set by {#link {@link #setPasswordMinimumSymbols(ComponentName, int)}
     * and only applies when the password quality is
     * {@link #PASSWORD_QUALITY_COMPLEX}.
     *
     * @param admin The name of the admin component to check, or null to
     *            aggregate all admins.
     * @return The minimum number of symbols required in the password.
     */
    CARAPI GetPasswordMinimumSymbols(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetPasswordMinimumSymbols(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
     * Called by an application that is administering the device to set the
     * minimum number of non-letter characters (numerical digits or symbols)
     * required in the password. After setting this, the user will not be able
     * to enter a new password that is not at least as restrictive as what has
     * been set. Note that the current password will remain until the user has
     * set a new one, so the change does not take place immediately. To prompt
     * the user for a new password, use {@link #ACTION_SET_NEW_PASSWORD} after
     * setting this value. This constraint is only imposed if the administrator
     * has also requested {@link #PASSWORD_QUALITY_COMPLEX} with
     * {@link #setPasswordQuality}. The default value is 0.
     * <p>
     * The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated
     *            with.
     * @param length The new desired minimum number of letters required in the
     *            password. A value of 0 means there is no restriction.
     */
    CARAPI SetPasswordMinimumNonLetter(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 length);

    /**
     * Retrieve the current number of non-letter characters required in the
     * password for all admins or a particular one. This is the same value as
     * set by {#link {@link #setPasswordMinimumNonLetter(ComponentName, int)}
     * and only applies when the password quality is
     * {@link #PASSWORD_QUALITY_COMPLEX}.
     *
     * @param admin The name of the admin component to check, or null to
     *            aggregate all admins.
     * @return The minimum number of letters required in the password.
     */
    CARAPI GetPasswordMinimumNonLetter(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetPasswordMinimumNonLetter(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
    * Called by an application that is administering the device to set the length
    * of the password history. After setting this, the user will not be able to
    * enter a new password that is the same as any password in the history. Note
    * that the current password will remain until the user has set a new one, so
    * the change does not take place immediately. To prompt the user for a new
    * password, use {@link #ACTION_SET_NEW_PASSWORD} after setting this value.
    * This constraint is only imposed if the administrator has also requested
    * either {@link #PASSWORD_QUALITY_NUMERIC},
    * {@link #PASSWORD_QUALITY_ALPHABETIC}, or
    * {@link #PASSWORD_QUALITY_ALPHANUMERIC} with {@link #setPasswordQuality}.
    *
    * <p>
    * The calling device admin must have requested
    * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call this
    * method; if it has not, a security exception will be thrown.
    *
    * @param admin Which {@link DeviceAdminReceiver} this request is associated
    *        with.
    * @param length The new desired length of password history. A value of 0
    *        means there is no restriction.
    */
    CARAPI SetPasswordHistoryLength(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 length);

    /**
     * Called by a device admin to set the password expiration timeout. Calling this method
     * will restart the countdown for password expiration for the given admin, as will changing
     * the device password (for all admins).
     *
     * <p>The provided timeout is the time delta in ms and will be added to the current time.
     * For example, to have the password expire 5 days from now, timeout would be
     * 5 * 86400 * 1000 = 432000000 ms for timeout.
     *
     * <p>To disable password expiration, a value of 0 may be used for timeout.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_EXPIRE_PASSWORD} to be able to call this
     * method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param timeout The limit (in ms) that a password can remain in effect. A value of 0
     *        means there is no restriction (unlimited).
     */
    CARAPI SetPasswordExpirationTimeout(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int64 timeout);

    /**
     * Get the password expiration timeout for the given admin. The expiration timeout is the
     * recurring expiration timeout provided in the call to
     * {@link #setPasswordExpirationTimeout(ComponentName, long)} for the given admin or the
     * aggregate of all policy administrators if admin is null.
     *
     * @param admin The name of the admin component to check, or null to aggregate all admins.
     * @return The timeout for the given admin or the minimum of all timeouts
     */
    CARAPI GetPasswordExpirationTimeout(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int64* password);

    /**
     * Get the current password expiration time for the given admin or an aggregate of
     * all admins if admin is null. If the password is expired, this will return the time since
     * the password expired as a negative number.  If admin is null, then a composite of all
     * expiration timeouts is returned - which will be the minimum of all timeouts.
     *
     * @param admin The name of the admin component to check, or null to aggregate all admins.
     * @return The password expiration time, in ms.
     */
    CARAPI GetPasswordExpiration(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int64* password);

    /**
     * Retrieve the current password history length for all admins
     * or a particular one.
     * @param admin The name of the admin component to check, or null to aggregate
     * all admins.
     * @return The length of the password history
     */
    CARAPI GetPasswordHistoryLength(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetPasswordHistoryLength(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
     * Return the maximum password length that the device supports for a
     * particular password quality.
     * @param quality The quality being interrogated.
     * @return Returns the maximum length that the user can enter.
     */
    CARAPI GetPasswordMaximumLength(
        /* [in] */ Int32 quality,
        /* [out] */ Int32* password);

    /**
     * Determine whether the current password the user has set is sufficient
     * to meet the policy requirements (quality, minimum length) that have been
     * requested.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_LIMIT_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @return Returns true if the password meets the current requirements,
     * else false.
     */
    CARAPI IsActivePasswordSufficient(
        /* [out] */ Boolean* isSufficient);

    /**
     * Retrieve the number of times the user has failed at entering a
     * password since that last successful password entry.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_WATCH_LOGIN} to be able to call
     * this method; if it has not, a security exception will be thrown.
     */
    CARAPI GetCurrentFailedPasswordAttempts(
        /* [out] */ Int32* password);

    /**
     * Setting this to a value greater than zero enables a built-in policy
     * that will perform a device wipe after too many incorrect
     * device-unlock passwords have been entered.  This built-in policy combines
     * watching for failed passwords and wiping the device, and requires
     * that you request both {@link DeviceAdminInfo#USES_POLICY_WATCH_LOGIN} and
     * {@link DeviceAdminInfo#USES_POLICY_WIPE_DATA}}.
     *
     * <p>To implement any other policy (e.g. wiping data for a particular
     * application only, erasing or revoking credentials, or reporting the
     * failure to a server), you should implement
     * {@link DeviceAdminReceiver#onPasswordFailed(Context, android.content.Intent)}
     * instead.  Do not use this API, because if the maximum count is reached,
     * the device will be wiped immediately, and your callback will not be invoked.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param num The number of failed password attempts at which point the
     * device will wipe its data.
     */
    CARAPI SetMaximumFailedPasswordsForWipe(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 num);

    /**
     * Retrieve the current maximum number of login attempts that are allowed
     * before the device wipes itself, for all admins
     * or a particular one.
     * @param admin The name of the admin component to check, or null to aggregate
     * all admins.
     */
    CARAPI GetMaximumFailedPasswordsForWipe(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* password);

    /** @hide per-user version */
    CARAPI GetMaximumFailedPasswordsForWipe(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    /**
     * Force a new device unlock password (the password needed to access the
     * entire device, not for individual accounts) on the user.  This takes
     * effect immediately.
     * The given password must be sufficient for the
     * current password quality and length constraints as returned by
     * {@link #getPasswordQuality(ComponentName)} and
     * {@link #getPasswordMinimumLength(ComponentName)}; if it does not meet
     * these constraints, then it will be rejected and false returned.  Note
     * that the password may be a stronger quality (containing alphanumeric
     * characters when the requested quality is only numeric), in which case
     * the currently active quality will be increased to match.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_RESET_PASSWORD} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param password The new password for the user.
     * @param flags May be 0 or {@link #RESET_PASSWORD_REQUIRE_ENTRY}.
     * @return Returns true if the password was applied, or false if it is
     * not acceptable for the current constraints.
     */
    CARAPI ResetPassword(
        /* [in] */ const String& password,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* reset);

    /**
     * Called by an application that is administering the device to set the
     * maximum time for user activity until the device will lock.  This limits
     * the length that the user can set.  It takes effect immediately.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_FORCE_LOCK} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param timeMs The new desired maximum time to lock in milliseconds.
     * A value of 0 means there is no restriction.
     */
    CARAPI SetMaximumTimeToLock(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int64 timeMs);

    /**
     * Retrieve the current maximum time to unlock for all admins
     * or a particular one.
     * @param admin The name of the admin component to check, or null to aggregate
     * all admins.
     */
    CARAPI GetMaximumTimeToLock(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int64* time);

    /** @hide per-user version */
    CARAPI GetMaximumTimeToLock(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int64* time);

    /**
     * Make the device lock immediately, as if the lock screen timeout has
     * expired at the point of this call.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_FORCE_LOCK} to be able to call
     * this method; if it has not, a security exception will be thrown.
     */
    CARAPI LockNow();

    /**
     * Ask the user date be wiped.  This will cause the device to reboot,
     * erasing all user data while next booting up.  External storage such
     * as SD cards will be also erased if the flag {@link #WIPE_EXTERNAL_STORAGE}
     * is set.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_WIPE_DATA} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param flags Bit mask of additional options: currently 0 and
     *              {@link #WIPE_EXTERNAL_STORAGE} are supported.
     */
    CARAPI WipeData(
        /* [in] */ Int32 flags);

    /**
     * Called by an application that is administering the device to set the
     * global proxy and exclusion list.
     * <p>
     * The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_SETS_GLOBAL_PROXY} to be able to call
     * this method; if it has not, a security exception will be thrown.
     * Only the first device admin can set the proxy. If a second admin attempts
     * to set the proxy, the {@link ComponentName} of the admin originally setting the
     * proxy will be returned. If successful in setting the proxy, null will
     * be returned.
     * The method can be called repeatedly by the device admin alrady setting the
     * proxy to update the proxy and exclusion list.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated
     *            with.
     * @param proxySpec the global proxy desired. Must be an HTTP Proxy.
     *            Pass Proxy.NO_PROXY to reset the proxy.
     * @param exclusionList a list of domains to be excluded from the global proxy.
     * @return returns null if the proxy was successfully set, or a {@link ComponentName}
     *            of the device admin that sets thew proxy otherwise.
     * @hide
     */
    CARAPI SetGlobalProxy(
        /* [in] */ IComponentName* admin,
        /* [in] */ Elastos::Net::IProxy* proxySpec,
        /* [in] */ IList* exclusionList,
        /* [out] */ IComponentName** component);

    /**
     * Set a network-independent global HTTP proxy.  This is not normally what you want
     * for typical HTTP proxies - they are generally network dependent.  However if you're
     * doing something unusual like general internal filtering this may be useful.  On
     * a private network where the proxy is not accessible, you may break HTTP using this.
     *
     * <p>This method requires the caller to be the device owner.
     *
     * <p>This proxy is only a recommendation and it is possible that some apps will ignore it.
     * @see ProxyInfo
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated
     *            with.
     * @param proxyInfo The a {@link ProxyInfo} object defining the new global
     *        HTTP proxy.  A {@code null} value will clear the global HTTP proxy.
     */
    CARAPI SetRecommendedGlobalProxy(
        /* [in] */ IComponentName* admin,
        /* [in] */ IProxyInfo* proxyInfo);

    /**
     * Returns the component name setting the global proxy.
     * @return ComponentName object of the device admin that set the global proxy, or
     *            null if no admin has set the proxy.
     * @hide
     */
    CARAPI GetGlobalProxyAdmin(
        /* [out] */ IComponentName** global);

    /**
     * Called by an application that is administering the device to
     * request that the storage system be encrypted.
     *
     * <p>When multiple device administrators attempt to control device
     * encryption, the most secure, supported setting will always be
     * used.  If any device administrator requests device encryption,
     * it will be enabled;  Conversely, if a device administrator
     * attempts to disable device encryption while another
     * device administrator has enabled it, the call to disable will
     * fail (most commonly returning {@link #ENCRYPTION_STATUS_ACTIVE}).
     *
     * <p>This policy controls encryption of the secure (application data) storage area.  Data
     * written to other storage areas may or may not be encrypted, and this policy does not require
     * or control the encryption of any other storage areas.
     * There is one exception:  If {@link android.os.Environment#isExternalStorageEmulated()} is
     * {@code true}, then the directory returned by
     * {@link android.os.Environment#getExternalStorageDirectory()} must be written to disk
     * within the encrypted storage area.
     *
     * <p>Important Note:  On some devices, it is possible to encrypt storage without requiring
     * the user to create a device PIN or Password.  In this case, the storage is encrypted, but
     * the encryption key may not be fully secured.  For maximum security, the administrator should
     * also require (and check for) a pattern, PIN, or password.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param encrypt true to request encryption, false to release any previous request
     * @return the new request status (for all active admins) - will be one of
     * {@link #ENCRYPTION_STATUS_UNSUPPORTED}, {@link #ENCRYPTION_STATUS_INACTIVE}, or
     * {@link #ENCRYPTION_STATUS_ACTIVE}.  This is the value of the requests;  Use
     * {@link #getStorageEncryptionStatus()} to query the actual device state.
     */
    CARAPI SetStorageEncryption(
        /* [in] */ IComponentName* admin,
        /* [in] */ Boolean encrypt,
        /* [out] */ Int32* result);

    /**
     * Called by an application that is administering the device to
     * determine the requested setting for secure storage.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.  If null,
     * this will return the requested encryption setting as an aggregate of all active
     * administrators.
     * @return true if the admin(s) are requesting encryption, false if not.
     */
    CARAPI GetStorageEncryption(
        /* [in] */ IComponentName* admin,
        /* [out] */ Boolean* result);

    /**
     * Called by an application that is administering the device to
     * determine the current encryption status of the device.
     *
     * Depending on the returned status code, the caller may proceed in different
     * ways.  If the result is {@link #ENCRYPTION_STATUS_UNSUPPORTED}, the
     * storage system does not support encryption.  If the
     * result is {@link #ENCRYPTION_STATUS_INACTIVE}, use {@link
     * #ACTION_START_ENCRYPTION} to begin the process of encrypting or decrypting the
     * storage.  If the result is {@link #ENCRYPTION_STATUS_ACTIVATING} or
     * {@link #ENCRYPTION_STATUS_ACTIVE}, no further action is required.
     *
     * @return current status of encryption.  The value will be one of
     * {@link #ENCRYPTION_STATUS_UNSUPPORTED}, {@link #ENCRYPTION_STATUS_INACTIVE},
     * {@link #ENCRYPTION_STATUS_ACTIVATING}, or{@link #ENCRYPTION_STATUS_ACTIVE}.
     */
    CARAPI GetStorageEncryptionStatus(
        /* [out] */ Int32* st);

    /** @hide per-user version */
    CARAPI GetStorageEncryptionStatus(
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* st);

    /**
     * Installs the given certificate as a user CA.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param certBuffer encoded form of the certificate to install.
     *
     * @return false if the certBuffer cannot be parsed or installation is
     *         interrupted, true otherwise.
     */
    CARAPI InstallCaCert(
        /* [in] */ IComponentName* admin,
        /* [in] */ ArrayOf<Byte>* certBuffer,
        /* [out] */ Boolean* result);

    /**
     * Uninstalls the given certificate from trusted user CAs, if present.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param certBuffer encoded form of the certificate to remove.
     */
    CARAPI UninstallCaCert(
        /* [in] */ IComponentName* admin,
        /* [in] */ ArrayOf<Byte>* certBuffer);

    /**
     * Returns all CA certificates that are currently trusted, excluding system CA certificates.
     * If a user has installed any certificates by other means than device policy these will be
     * included too.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @return a List of byte[] arrays, each encoding one user CA certificate.
     */
    CARAPI GetInstalledCaCerts(
        /* [in] */ IComponentName* admin,
        /* [out] */ IList** list);

    /**
     * Uninstalls all custom trusted CA certificates from the profile. Certificates installed by
     * means other than device policy will also be removed, except for system CA certificates.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     */
    CARAPI UninstallAllUserCaCerts(
        /* [in] */ IComponentName* admin);
    /**
     * Returns whether this certificate is installed as a trusted CA.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param certBuffer encoded form of the certificate to look up.
     */
    CARAPI HasCaCertInstalled(
        /* [in] */ IComponentName* admin,
        /* [in] */ ArrayOf<Byte>* certBuffer,
        /* [out] */ Boolean* result);

    /**
     * Called by a device or profile owner to install a certificate and private key pair. The
     * keypair will be visible to all apps within the profile.
     *
     * @param who Which {@link DeviceAdminReceiver} this request is associated with.
     * @param privKey The private key to install.
     * @param cert The certificate to install.
     * @param alias The private key alias under which to install the certificate. If a certificate
     * with that alias already exists, it will be overwritten.
     * @return {@code true} if the keys were installed, {@code false} otherwise.
     */
    CARAPI InstallKeyPair(
        /* [in] */ IComponentName* who,
        /* [in] */ IPrivateKey* privKey,
        /* [in] */ ICertificate* cert,
        /* [in] */ const String& alias,
        /* [out] */ Boolean* result);

    /**
     * Called by an application that is administering the device to disable all cameras
     * on the device.  After setting this, no applications will be able to access any cameras
     * on the device.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_DISABLE_CAMERA} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param disabled Whether or not the camera should be disabled.
     */
    CARAPI SetCameraDisabled(
        /* [in] */ IComponentName* admin,
        /* [in] */ Boolean disabled);

    /**
     * Determine whether or not the device's cameras have been disabled either by the current
     * admin, if specified, or all admins.
     * @param admin The name of the admin component to check, or null to check if any admins
     * have disabled the camera
     */
    CARAPI GetCameraDisabled(
        /* [in] */ IComponentName* admin,
        /* [out] */ Boolean* disabled);

    /** @hide per-user version */
    CARAPI GetCameraDisabled(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* disabled);

    /**
     * Called by a device/profile owner to set whether the screen capture is disabled. Disabling
     * screen capture also prevents the content from being shown on display devices that do not have
     * a secure video output. See {@link android.view.Display#FLAG_SECURE} for more details about
     * secure surfaces and secure displays.
     *
     * <p>The calling device admin must be a device or profile owner. If it is not, a
     * security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param disabled Whether screen capture is disabled or not.
     */
    CARAPI SetScreenCaptureDisabled(
        /* [in] */ IComponentName* admin,
        /* [in] */ Boolean disabled);

    /**
     * Determine whether or not screen capture has been disabled by the current
     * admin, if specified, or all admins.
     * @param admin The name of the admin component to check, or null to check if any admins
     * have disabled screen capture.
     */
    CARAPI GetScreenCaptureDisabled(
        /* [in] */ IComponentName* admin,
        /* [out] */ Boolean* result);

    /** @hide per-user version */
    CARAPI GetScreenCaptureDisabled(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Called by a device owner to set whether auto time is required. If auto time is
     * required the user cannot set the date and time, but has to use network date and time.
     *
     * <p>Note: if auto time is required the user can still manually set the time zone.
     *
     * <p>The calling device admin must be a device owner. If it is not, a security exception will
     * be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param required Whether auto time is set required or not.
     */
    CARAPI SetAutoTimeRequired(
        /* [in] */ IComponentName* admin,
        /* [in] */ Boolean required);

    /**
     * @return true if auto time is required.
     */
    CARAPI GetAutoTimeRequired(
        /* [out] */ Boolean* result);

    /**
     * Called by an application that is administering the device to disable keyguard customizations,
     * such as widgets. After setting this, keyguard features will be disabled according to the
     * provided feature list.
     *
     * <p>The calling device admin must have requested
     * {@link DeviceAdminInfo#USES_POLICY_DISABLE_KEYGUARD_FEATURES} to be able to call
     * this method; if it has not, a security exception will be thrown.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @param which {@link #KEYGUARD_DISABLE_FEATURES_NONE} (default),
     * {@link #KEYGUARD_DISABLE_WIDGETS_ALL}, {@link #KEYGUARD_DISABLE_SECURE_CAMERA},
     * {@link #KEYGUARD_DISABLE_FEATURES_ALL}
     */
    CARAPI SetKeyguardDisabledFeatures(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 which);

    /**
     * Determine whether or not features have been disabled in keyguard either by the current
     * admin, if specified, or all admins.
     * @param admin The name of the admin component to check, or null to check if any admins
     * have disabled features in keyguard.
     * @return bitfield of flags. See {@link #setKeyguardDisabledFeatures(ComponentName, int)}
     * for a list.
     */
    CARAPI GetKeyguardDisabledFeatures(
        /* [in] */ IComponentName* admin,
        /* [out] */ Int32* which);

    /** @hide per-user version */
    CARAPI GetKeyguardDisabledFeatures(
        /* [in] */ IComponentName* admin,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* which);

    /**
     * @hide
     */
    CARAPI SetActiveAdmin(
        /* [in] */ IComponentName* policyReceiver,
        /* [in] */ Boolean refreshing);

    CARAPI SetActiveAdmin(
        /* [in] */ IComponentName* policyReceiver,
        /* [in] */ Boolean refreshing,
        /* [in] */ Int32 userId);

    /**
     * Returns the DeviceAdminInfo as defined by the administrator's package info & meta-data
     * @hide
     */
    CARAPI GetAdminInfo(
        /* [in] */ IComponentName* cn,
        /* [out] */ IDeviceAdminInfo** info);

    /**
     * @hide
     */
    CARAPI GetRemoveWarning(
        /* [in] */ IComponentName* admin,
        /* [in] */ IRemoteCallback* result);

    /**
     * @hide
     */
    CARAPI SetActivePasswordState(
        /* [in] */ Int32 quality,
        /* [in] */ Int32 length,
        /* [in] */ Int32 letters,
        /* [in] */ Int32 uppercase,
        /* [in] */ Int32 lowercase,
        /* [in] */ Int32 numbers,
        /* [in] */ Int32 symbols,
        /* [in] */ Int32 nonletter,
        /* [in] */ Int32 userHandle);

    /**
     * @hide
     */
    CARAPI ReportFailedPasswordAttempt(
        /* [in] */ Int32 userHandle);

    /**
     * @hide
     */
    CARAPI ReportSuccessfulPasswordAttempt(
        /* [in] */ Int32 userHandle);

    CARAPI SetDeviceOwner(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI SetDeviceOwner(
        /* [in] */ const String& packageName,
        /* [in] */ const String& ownerName,
        /* [out] */ Boolean* result);

    CARAPI IsDeviceOwnerApp(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI IsDeviceOwner(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI ClearDeviceOwnerApp(
        /* [in] */ const String& packageName);

    /** @hide */
    // @SystemApi
    CARAPI GetDeviceOwner(
        /* [out] */ String* result);

    /** @hide */
    CARAPI GetDeviceOwnerName(
        /* [out] */ String* result);

    /**
     * @hide
     * @deprecated Use #ACTION_SET_PROFILE_OWNER
     * Sets the given component as an active admin and registers the package as the profile
     * owner for this user. The package must already be installed and there shouldn't be
     * an existing profile owner registered for this user. Also, this method must be called
     * before the user setup has been completed.
     * <p>
     * This method can only be called by system apps that hold MANAGE_USERS permission and
     * MANAGE_DEVICE_ADMINS permission.
     * @param admin The component to register as an active admin and profile owner.
     * @param ownerName The user-visible name of the entity that is managing this user.
     * @return whether the admin was successfully registered as the profile owner.
     * @throws IllegalArgumentException if packageName is null, the package isn't installed, or
     *         the user has already been set up.
     */
    // @SystemApi
    CARAPI SetActiveProfileOwner(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& ownerName,
        /* [out] */ Boolean* result);

    /**
     * @hide
     * Clears the active profile owner and removes all user restrictions. The caller must
     * be from the same package as the active profile owner for this user, otherwise a
     * SecurityException will be thrown.
     *
     * @param admin The component to remove as the profile owner.
     * @return
     */
    // @SystemApi
    CARAPI ClearProfileOwner(
        /* [in] */ IComponentName* admin);

    /**
     * @hide
     * Checks if the user was already setup.
     */
    CARAPI HasUserSetupCompleted(
        /* [out] */ Boolean* result);

    CARAPI SetProfileOwner(
        /* [in] */ const String& packageName,
        /* [in] */ const String& ownerName,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    CARAPI SetProfileOwner(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& ownerName,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    CARAPI SetProfileEnabled(
        /* [in] */ IComponentName* admin);

    CARAPI SetProfileName(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& profileName);

    CARAPI IsProfileOwnerApp(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI GetProfileOwner(
        /* [out]*/ IComponentName** name);

    /**
     * @see #getProfileOwner()
     * @hide
     */
    CARAPI GetProfileOwnerAsUser(
        /* [in] */ Int32 userId,
        /* [out]*/ IComponentName** name);

    /**
     * @hide
     * @return the human readable name of the organisation associated with this DPM or null if
     *         one is not set.
     * @throws IllegalArgumentException if the userId is invalid.
     */
    CARAPI GetProfileOwnerName(
        /* [out]*/ String* result);

    /**
     * @hide
     * @param user The user for whom to fetch the profile owner name, if any.
     * @return the human readable name of the organisation associated with this profile owner or
     *         null if one is not set.
     * @throws IllegalArgumentException if the userId is invalid.
     */
    //@SystemApi
    CARAPI GetProfileOwnerNameAsUser(
        /* [in] */ Int32 userId,
        /* [out] */ String* result);

    CARAPI AddPersistentPreferredActivity(
        /* [in] */ IComponentName* admin,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ IComponentName* activity);

    CARAPI ClearPackagePersistentPreferredActivities(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName);

    CARAPI SetApplicationRestrictions(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [in] */ IBundle* settings);

    CARAPI SetTrustAgentFeaturesEnabled(
        /* [in] */ IComponentName* admin,
        /* [in] */ IComponentName* agent,
        /* [in] */ IList* features);

    CARAPI GetTrustAgentFeaturesEnabled(
        /* [in] */ IComponentName* admin,
        /* [in] */ IComponentName* agent,
        /* [out] */ IList** result);

    CARAPI SetCrossProfileCallerIdDisabled(
        /* [in] */ IComponentName* who,
        /* [in] */ Boolean disabled);

    CARAPI GetCrossProfileCallerIdDisabled(
        /* [in] */ IComponentName* who,
        /* [out] */ Boolean* result);

    /**
     * Determine whether or not caller-Id information has been disabled.
     *
     * @param userHandle The user for whom to check the caller-id permission
     * @hide
     */
    CARAPI GetCrossProfileCallerIdDisabled(
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ Boolean* result);

    CARAPI AddCrossProfileIntentFilter(
        /* [in] */ IComponentName* admin,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 flags);

    CARAPI ClearCrossProfileIntentFilters(
        /* [in] */ IComponentName* admin);

    CARAPI SetPermittedAccessibilityServices(
        /* [in] */ IComponentName* admin,
        /* [in] */ IList* packageNames,
        /* [out] */ Boolean* result);

    CARAPI GetPermittedAccessibilityServices(
        /* [in] */ IComponentName* admin,
        /* [out] */ IList** result);

    /**
     * Returns the list of accessibility services permitted by the device or profiles
     * owners of this user.
     *
     * <p>Null means all accessibility services are allowed, if a non-null list is returned
     * it will contain the intersection of the permitted lists for any device or profile
     * owners that apply to this user. It will also include any system accessibility services.
     *
     * @param userId which user to check for.
     * @return List of accessiblity service package names.
     * @hide
     */
     // @SystemApi
    CARAPI GetPermittedAccessibilityServices(
        /* [in] */ Int32 userId,
        /* [out] */ IList** result);

    CARAPI SetPermittedInputMethods(
        /* [in] */ IComponentName* admin,
        /* [in] */ IList* packageNames,
        /* [out] */ Boolean* result);

    CARAPI GetPermittedInputMethods(
        /* [in] */ IComponentName* admin,
        /* [out] */ IList** list);

    /**
     * Returns the list of input methods permitted by the device or profiles
     * owners of the current user.
     *
     * <p>Null means all input methods are allowed, if a non-null list is returned
     * it will contain the intersection of the permitted lists for any device or profile
     * owners that apply to this user. It will also include any system input methods.
     *
     * @return List of input method package names.
     * @hide
     */
    // @SystemApi
    CARAPI GetPermittedInputMethodsForCurrentUser(
        /* [out] */ IList** list);

    CARAPI CreateUser(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& name,
        /* [out] */ IUserHandle** handle);

    CARAPI CreateAndInitializeUser(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& name,
        /* [in] */ const String& ownerName,
        /* [in] */ IComponentName* profileOwnerComponent,
        /* [in] */ IBundle* adminExtras,
        /* [out] */ IUserHandle** handle);

    CARAPI RemoveUser(
        /* [in] */ IComponentName* admin,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ Boolean* result);

    CARAPI SwitchUser(
        /* [in] */ IComponentName* admin,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ Boolean* result);

    CARAPI GetApplicationRestrictions(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [out] */ IBundle** bundle);

    CARAPI AddUserRestriction(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& key);

    CARAPI ClearUserRestriction(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& key);

    CARAPI SetApplicationHidden(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean hide,
        /* [out] */ Boolean* result);

    CARAPI IsApplicationHidden(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI EnableSystemApp(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName);

    CARAPI EnableSystemApp(
        /* [in] */ IComponentName* admin,
        /* [in] */ IIntent* intent,
        /* [out] */ Int32* result);

    CARAPI SetAccountManagementDisabled(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& accountType,
        /* [in] */ Boolean disabled);

    CARAPI GetAccountTypesWithManagementDisabled(
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * @see #getAccountTypesWithManagementDisabled()
     * @hide
     */
    CARAPI GetAccountTypesWithManagementDisabledAsUser(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI SetLockTaskPackages(
        /* [in] */ IComponentName* admin,
        /* [in] */ ArrayOf<String>* packages);

    /**
     * This function returns the list of packages allowed to start the lock task mode.
     *
     * @param admin Which {@link DeviceAdminReceiver} this request is associated with.
     * @hide
     */
    CARAPI GetLockTaskPackages(
        /* [in] */ IComponentName* admin,
        /* [out, callee] */ ArrayOf<String>** packages);

    CARAPI IsLockTaskPermitted(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* result);

    CARAPI SetGlobalSetting(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& setting,
        /* [in] */ const String& value);

    CARAPI SetSecureSetting(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& setting,
        /* [in] */ const String& value);

    CARAPI SetRestrictionsProvider(
        /* [in] */ IComponentName* admin,
        /* [in] */ IComponentName* provider);

    CARAPI SetMasterVolumeMuted(
        /* [in] */ IComponentName* admin,
        /* [in] */ Boolean on);

    CARAPI IsMasterVolumeMuted(
        /* [in] */ IComponentName* admin,
        /* [out] */ Boolean* result);

    CARAPI SetUninstallBlocked(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean uninstallBlocked);

    CARAPI IsUninstallBlocked(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI AddCrossProfileWidgetProvider(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI RemoveCrossProfileWidgetProvider(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI GetCrossProfileWidgetProviders(
        /* [in] */ IComponentName* admin,
        /* [out] */ IList** result);

    CARAPI RequireSecureKeyguard(
        /* [out] */ Boolean* isRequired);

    CARAPI RequireSecureKeyguard(
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* isRequired);

private:
    /**
     * Returns the alias of a given CA certificate in the certificate store, or null if it
     * doesn't exist.
     */
    static CARAPI GetCaCertAlias(
        /* [in] */ ArrayOf<Byte>* certBuffer,
        /* [out] */ String* alias);

private:
    static const String TAG;
    IContext* mContext;
    AutoPtr<IIDevicePolicyManager> mService;
};

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ADMIN_CDEVICEPOLICYMANAGER_H__
